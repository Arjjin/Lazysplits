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
	//if( IsTargets() ){
	if( frame_buf_->FrameCount() > 0 ){
		std::shared_ptr<cv::Mat> frame = frame_buf_->PeekFrame();

		try{
		cv::Mat BGR_frame;
		cv::cvtColor( *frame, BGR_frame, CV_RGBA2BGR );

		/*
		for( auto target_it = target_list_.begin(); target_it != target_list_.end(); ++target_it ){
			//blog( LOG_DEBUG, "[Lazysplits][%s] %s target", thread_name_.c_str(), (*target_it)->GetName().c_str() );
			auto watch_list = (*target_it)->GetCurrentWatches();
			for( auto watch_it = watch_list.begin(); watch_it != watch_list.end(); ++watch_it ){
				const cv::Mat& watch_img = (*watch_it)->GetImage( calib_props_ );
				cv::imwrite( "./images/watch.png", watch_img, compression_params );
				//blog( LOG_DEBUG, "[Lazysplits][%s] %s watch", thread_name_.c_str(), watch_it->GetName().c_str() );
				
			}
		}
		*/
		
		//cv compression params
		std::vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(1);
		cv::imwrite( "./images/frame.png", BGR_frame, compression_params );
		}
		catch( cv::Exception cve ){
			blog( LOG_ERROR, "[lazysplits][%s] failed to process frame; %s!", thread_name_.c_str(), cve.msg.c_str() );
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
			case CsMsg.NEW_TARGET :
				NewTarget(CsMsg);
			break;
		}
	}
}

void LzsCvThread::HandleCalibrationData( std::shared_ptr<CvMsg> msg ){
	std::shared_ptr<CvCalibrationDataMsg> calib_msg = std::static_pointer_cast<CvCalibrationDataMsg>(msg);
	calib_props_ = calib_msg->calib_props_;
	blog( LOG_DEBUG, "[lazysplits][%s] calibration %s; img dims : (%ix%i), offset : (%i,%i), scale : (%f,%f)",
		thread_name_.c_str(),
		calib_props_.is_enabled ? "enabled" : "disabled",
		calib_props_.img_width,
		calib_props_.img_height,
		calib_props_.loc_x,
		calib_props_.loc_y,
		calib_props_.scale_x,
		calib_props_.scale_y
	);
}

void LzsCvThread::NewTarget( Proto::CsMessage& msg ){
	//make sure shared data directory is the same as our LiveSplit plugin
	if( shared_data_manager_.IsMatchingRootDir( msg.shared_data_dir() ) ){
		std::shared_ptr<SharedData::LzsTarget> target;
		if( shared_data_manager_.TryConstructTarget( msg.game_name(), msg.target_name(), target ) ){
			target_list_.push_back(target);
			blog( LOG_DEBUG, "[lazysplits][%s] target added; name : %s", thread_name_.c_str(), target->GetName().c_str() );
		}
	}
	else{
		blog( LOG_WARNING, "[lazysplits][%s] mismatched shared data dirs! OBS : %s, LS : %s", thread_name_.c_str(), shared_data_manager_.GetRootDir().c_str(), msg.shared_data_dir().c_str() );
		return;
	}
}

} //namespace Lazysplits