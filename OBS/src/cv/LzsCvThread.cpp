#include "LzsCvThread.h"
#include "util\LzsImgProc.h"
#include "util\LzsProtoHelper.h"
#include "util\LzsSharedData.h"

#include <obs.h>
#include <opencv2\highgui.hpp>

namespace Lazysplits{

LzsCvThread::LzsCvThread( LzsFrameBuffer* frame_buf )
	:LzsThread("CV thread"),
	 msg_queue_("CV queue")
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	frame_buf_ = frame_buf;
	pipe_message_id_ref_ = 0;
}

void LzsCvThread::AssignPipe( LzsPipeServer* pipe ){ pipe_ = pipe; }

bool LzsCvThread::IsCvActive(){ return is_cv_active_; }

/* message queue helpers */

void LzsCvThread::MsgPipeConnected( bool connected ){
	msg_queue_.Push( std::make_shared<CvPipeConnectionMsg>(connected) );
}

void LzsCvThread::MsgSetSharedDataPath( std::string path ){
	msg_queue_.Push( std::make_shared<CvSharedPathMsg>(path) );
}

void LzsCvThread::MsgProtobuf( std::string serialized_protobuf ){
	msg_queue_.Push( std::make_shared<CvPipeProtobufMsg>(serialized_protobuf) );
}

void LzsCvThread::OnSubjectNotify( const std::string& subject_name, const std::string& subject_message ){
	if( IsThreadSleeping() ){ ThreadWake(); }
}

void LzsCvThread::ThreadFuncInit(){
	LzsThread::ThreadFuncInit();

	//subscribe to message queue and frame buffer
	msg_queue_.AttachObserver(this);
	frame_buf_->AttachObserver(this);

	is_cv_active_ = false;
}

void* LzsCvThread::ThreadFunc(){
	ThreadFuncInit();

	while( ThreadFuncShouldLoop() ){
		//check for messages from pipe
		if( !msg_queue_.IsEmpty() ){
			HandleMessageQueue();
		}
		//handle one frame per loop in frame buffer
		else if( is_cv_active_ && frame_buf_->FrameCount() > 0 ){
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
	
	is_cv_active_ = false;

	LzsThread::ThreadFuncCleanup();
}

void LzsCvThread::ActivateCv(){ is_cv_active_ = true; }
void LzsCvThread::DeactivateCv(){ is_cv_active_ = false; }

void LzsCvThread::HandleFrameBuffer(){
	obs_source_frame* frame  = frame_buf_->PeekFrame();
	if( frame ){
		cv::Mat BGR_frame;
		BGR_frame = ImgProc::FrameToBGRMat(frame);

		/*
		//cv compression params
		std::vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(1);

		cv::imwrite( "../img.png", BGR_frame, compression_params );
		*/
	}
	else{ blog( LOG_WARNING, "[lazysplits][%s] frame buffer peek is bad", thread_name_.c_str() ); }
	frame_buf_->PopFrame();
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
		}

		msg_queue_.Pop();
	}
}

void LzsCvThread::HandlePipeConnected( std::shared_ptr<CvMsg> msg ){
	std::shared_ptr<CvPipeConnectionMsg> connection_msg = std::static_pointer_cast<CvPipeConnectionMsg>(msg);
	std::string status = (connection_msg->pipe_connected_) ? "Connected" : "Disconnected";
	blog( LOG_DEBUG, "[lazysplits][%s] pipe %s", thread_name_.c_str(), status.c_str() );
}

void LzsCvThread::HandleSetSharedDataPath( std::shared_ptr<CvMsg> msg ){
	std::shared_ptr<CvSharedPathMsg> connection_msg = std::static_pointer_cast<CvSharedPathMsg>(msg);

	shared_data_dir_ = connection_msg->shared_data_path_;
	std::string game_list_found = SharedData::PathHasGameList(shared_data_dir_) ? "valid" : "invalid";

	blog( LOG_DEBUG, "[lazysplits][%s] new shared data path is %s", thread_name_.c_str(), game_list_found.c_str() );
}

void LzsCvThread::HandleProtobuf( std::shared_ptr<CvMsg> msg ){
	std::shared_ptr<CvPipeProtobufMsg> connection_msg = std::static_pointer_cast<CvPipeProtobufMsg>(msg);

}

} //namespace Lazysplits