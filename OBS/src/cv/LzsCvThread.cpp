#include "LzsCvThread.h"
#include "util\LzsImgProc.h"
#include "util\LzsProtoHelper.h"

#include <obs.h>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

namespace Lazysplits{

LzsCvThread::LzsCvThread( LzsFrameBuffer* frame_buf, obs_source_t* context )
	:LzsThread("CV thread"),
	 msg_queue_("CV queue")
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	pipe_connected_ = false;

	context_ = context;
	frame_buf_ = frame_buf;
	ls_msg_id_ = 0;

	calib_props_.is_enabled = false;
}

void LzsCvThread::AssignPipe( LzsPipeServer* pipe ){ pipe_ = pipe; }

bool LzsCvThread::IsTargets(){ return target_list_.size() > 0; }

/* message queue helpers */

void LzsCvThread::MsgPipeConnected( bool connected ){
	msg_queue_.Push( std::make_shared<CvPipeConnectionMsg>(connected) );
}

void LzsCvThread::MsgSetSharedDataPath( const std::string& path ){
	msg_queue_.Push( std::make_shared<CvSharedPathMsg>(path) );
}

void LzsCvThread::MsgProtobuf( std::string serialized_protobuf ){
	msg_queue_.Push( std::make_shared<CvPipeProtobufMsg>(serialized_protobuf) );
}

void LzsCvThread::MsgCalibData( const SendableCalibrationProps& calib_props ){
	msg_queue_.Push( std::make_shared<CvCalibrationDataMsg>(calib_props) );
}

void LzsCvThread::OnSubjectNotify( const std::string& subject_name, const std::string& subject_message ){
	if( IsThreadSleeping() ){ ThreadWake(); }
}

void LzsCvThread::ThreadFuncInit(){
	LzsThread::ThreadFuncInit();

	//subscribe to message queue and frame buffer
	msg_queue_.AttachObserver(this);
	frame_buf_->AttachObserver(this);
}

void* LzsCvThread::ThreadFunc(){
	ThreadFuncInit();

	while( ThreadFuncShouldLoop() ){
		//check for messages from pipe
		if( !msg_queue_.IsEmpty() ){
			HandleMessageQueue();
		}
		//handle one frame per loop in frame buffer
		else if( frame_buf_->FrameCount() > 0 ){
			HandleFrameBuffer();
		}
		//no work to do, go to sleep
		else{ ThreadSleep(); }

	}

	ThreadFuncCleanup();
	return NULL;
}

void LzsCvThread::ThreadFuncCleanup(){
	msg_queue_.DetachObserver(this);
	frame_buf_->DetachObserver(this);

	LzsThread::ThreadFuncCleanup();
}

void LzsCvThread::HandleFrameBuffer(){
	if( frame_buf_->FrameCount() > 0 && IsTargets() ){
		LzsFrame frame = frame_buf_->PeekFrame();

		try{
			cv::Mat BGR_frame;
			cv::cvtColor( *frame.frame_mat_, BGR_frame, cv::COLOR_RGBA2BGR );
			
			//cv compression params
			std::vector<int> compression_params;
			compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
			compression_params.push_back(1);

			auto target_it = target_list_.begin();
			while( target_it != target_list_.end() ){
				std::shared_ptr<SharedData::LzsTarget> current_target = (*target_it);
				
				auto watch_list = current_target->GetCurrentWatches();
				bool target_found = false;
				for( auto watch_it = watch_list.begin(); watch_it != watch_list.end(); ++watch_it ){
					std::shared_ptr<SharedData::LzsWatchBase> current_watch = (*watch_it);

					if( current_watch->FindWatch( BGR_frame, calib_props_ ) ){
						std::stringstream fn;
						fn << "./images/found_" << current_watch->GetName().c_str() << ".png";

						blog( LOG_DEBUG, "[lazysplits][%s] %s watch found for threshold %f", thread_name_.c_str(),
							current_watch->GetName().c_str(),
							current_watch->GetThreshold()
						);
						cv::imwrite( fn.str().c_str(), BGR_frame, compression_params );

						current_target->WatchAction( current_watch->GetAction(), current_watch->GetActionVal() );
						break;
					}
				}
				
				if( current_target->IsComplete() ){
					LsMsgTargetFound( shared_data_manager_.GetGameName(), (*target_it)->GetName(), frame.timestamp_, (*target_it)->GetSplitOffset() );
					target_it = target_list_.erase(target_it);
				}
				else{
					++target_it;
				}
			}
		}
		catch( cv::Exception cve ){
			blog( LOG_ERROR, "[lazysplits][%s] failed to process frame; %s!", thread_name_.c_str(), cve.msg.c_str() );
			throw cve;
		}
		frame_buf_->PopFrame();
	}

}

/* pipe to livesplit message queue helpers */

int32_t LzsCvThread::GetLsMsgId(){
	return ls_msg_id_++;
}

void LzsCvThread::LsMsgRequestResync(){
	Proto::CppMessage msg;
	msg.set_id( GetLsMsgId() );
	msg.set_type( Proto::CppMessage_MessageType::CppMessage_MessageType_REQUEST_RESYNC );

	std::string serialized_msg;
	if( msg.SerializeToString(&serialized_msg) ){
		pipe_->MsgProtobuf(serialized_msg);
	}
	else{ blog( LOG_WARNING, "[lazysplits][%s] failed to serialize outgoing resync request!", thread_name_.c_str() ); }
}

void LzsCvThread::LsMsgTargetFound( const std::string& game_name, const std::string& target_name, uint64_t timestamp, uint64_t split_offset ){
	Proto::CppMessage msg;
	msg.set_id( GetLsMsgId() );
	msg.set_type( Proto::CppMessage_MessageType::CppMessage_MessageType_TARGET_FOUND );
	msg.set_shared_data_dir( shared_data_manager_.GetRootDir() );
	msg.set_game_name( shared_data_manager_.GetGameName() );
	msg.set_target_name(target_name);
	msg.set_target_timestamp(timestamp);
	
	std::string serialized_msg;
	if( msg.SerializeToString(&serialized_msg) ){
		pipe_->MsgProtobuf(serialized_msg);
	}
	else{ blog( LOG_WARNING, "[lazysplits][%s] failed to serialize outgoing target found message!", thread_name_.c_str() ); }
}

/* message queue handling */

void LzsCvThread::HandleMessageQueue(){
	while( !should_terminate_ && !msg_queue_.IsEmpty() ){
		std::shared_ptr<CvMsg> msg = msg_queue_.Peek();

		switch(msg->type_){
			case CV_PIPE_CONNECTION_MSG :
				HandlePipeConnected(msg);
			break;
			case CV_PIPE_PROTOBUF_MSG :
				HandleProtobuf(msg);
			break;
			case CV_SHARED_DATA_PATH_MSG :
				HandleSetSharedDataPath(msg);
			break;
			case CV_CALIBRATION_DATA_MSG :
				HandleCalibrationData(msg);
			break;
		}

		msg_queue_.Pop();
	}
}

void LzsCvThread::HandlePipeConnected( std::shared_ptr<CvMsg> msg ){
	std::shared_ptr<CvPipeConnectionMsg> connection_msg = std::static_pointer_cast<CvPipeConnectionMsg>(msg);
	//request targets if we connected, clear our target list if we disconnected
	( pipe_connected_ = connection_msg->pipe_connected_ ) ? LsMsgRequestResync() : target_list_.clear();
	blog( LOG_DEBUG, "[lazysplits][%s] pipe %s", thread_name_.c_str(), pipe_connected_ ? "Connected" : "Disconnected" );
}

void LzsCvThread::HandleSetSharedDataPath( std::shared_ptr<CvMsg> msg ){
	std::shared_ptr<CvSharedPathMsg> shared_data_path_msg = std::static_pointer_cast<CvSharedPathMsg>(msg);

	shared_data_manager_.SetRootDir(shared_data_path_msg->shared_data_path_);

	target_list_.clear();
	LsMsgRequestResync();
}

void LzsCvThread::HandleProtobuf( std::shared_ptr<CvMsg> msg ){
	std::shared_ptr<CvPipeProtobufMsg> protobuf_msg = std::static_pointer_cast<CvPipeProtobufMsg>(msg);
	Proto::CsMessage CsMsg;

	if( CsMsg.ParseFromString(protobuf_msg->serialized_protobuf_) ){

		switch( CsMsg.type() ){
			case CsMsg.CLEAR_TARGETS :
				target_list_.clear();
				blog( LOG_DEBUG, "[lazysplits][%s] cleared targets", thread_name_.c_str() );
			break;
			case CsMsg.NEW_TARGETS :
				NewTargets(CsMsg);
			break;
		}
	}
}

void LzsCvThread::HandleCalibrationData( std::shared_ptr<CvMsg> msg ){
	std::shared_ptr<CvCalibrationDataMsg> calib_msg = std::static_pointer_cast<CvCalibrationDataMsg>(msg);
	calib_props_ = calib_msg->calib_props_;
	blog( LOG_DEBUG, "[lazysplits][%s] calibration %s; img dims : (%ix%i), offset : (%i,%i), scale : (%f,%f), use NN interp : %i",
		thread_name_.c_str(),
		calib_props_.is_enabled ? "enabled" : "disabled",
		calib_props_.img_width,
		calib_props_.img_height,
		calib_props_.loc_x,
		calib_props_.loc_y,
		calib_props_.scale_x,
		calib_props_.scale_y,
		calib_props_.use_nn_interp
	);
}

void LzsCvThread::NewTargets( Proto::CsMessage& msg ){
	//make sure shared data directory is the same as our LiveSplit plugin
	if( shared_data_manager_.IsMatchingRootDir( msg.shared_data_dir() ) ){
		shared_data_manager_.TryConstructTargetList( msg, target_list_ );
	}
	else{
		blog(
			LOG_WARNING,
			"[lazysplits][%s] mismatched shared data dirs! OBS : %s, LS : %s",
			thread_name_.c_str(),
			shared_data_manager_.GetRootDir().c_str(),
			msg.shared_data_dir().c_str()
		);
		return;
	}
}

} //namespace Lazysplits