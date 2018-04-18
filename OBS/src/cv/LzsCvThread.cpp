#include "LzsCvThread.h"
#include "util\LzsImgProc.h"
#include "util\LzsProtoHelper.h"

#include <obs.h>
#include <opencv2\highgui.hpp>

namespace Lazysplits{

LzsCvThread::LzsCvThread( LzsFrameBuffer* frame_buf, LzsMessageQueue<std::string>* cv_to_pipe_queue, LzsMessageQueue<std::string>* pipe_to_cv_queue )
	:LzsThread("CV thread")
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	frame_buf_ = frame_buf;
	cv_to_pipe_queue_ = cv_to_pipe_queue;
	pipe_to_cv_queue_ = pipe_to_cv_queue;
	should_read_queue_ = false;
	is_cv_active_ = false;
	message_id_ref_ = 0;
}

bool LzsCvThread::IsCvActive(){ return is_cv_active_; }

void LzsCvThread::OnSubjectNotify( std::string subject_name, std::string subject_message ){
	if( subject_name == "cv queue"){
		should_read_queue_ = true;
	}
	else if( subject_name == "Pipe"){
		if( subject_message == "Connected"){
			blog( LOG_DEBUG, "[lazysplits][%s] pipe connected", thread_name_.c_str() ); 
		}
		else if( subject_message == "Disconnected"){
			blog( LOG_DEBUG, "[lazysplits][%s] pipe disconnected", thread_name_.c_str() ); 
		}
	}

	if( IsThreadSleeping() ){ ThreadWake(); }
}

void LzsCvThread::ThreadFuncInit(){
	LzsThread::ThreadFuncInit();
	
	//subscribe to message queue and frame buffer
	pipe_to_cv_queue_->AttachObserver(this);
	frame_buf_->AttachObserver(this);
}

void* LzsCvThread::ThreadFunc(){
	ThreadFuncInit();

	while( ThreadFuncShouldLoop() ){
		//check for messages from pipe
		if( should_read_queue_ ){
			HandleMessages();
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
	pipe_to_cv_queue_->DetachObserver(this);
	frame_buf_->DetachObserver(this);

	LzsThread::ThreadFuncCleanup();
}

void LzsCvThread::ActivateCv(){ is_cv_active_ = true; }
void LzsCvThread::DeactivateCv(){ is_cv_active_ = false; }

void LzsCvThread::HandleMessages(){
	Proto::CppMessage message;

	while( !pipe_to_cv_queue_->QueueIsEmpty() ){
		std::string message_string = pipe_to_cv_queue_->QueueFront();
		if( message.ParseFromString(message_string) ){
			blog( LOG_DEBUG, "[lazysplits][%s] deserialized protobuf of type : %i", thread_name_.c_str(), message.message_type() ); 
		}
		else{
			blog( LOG_WARNING, "[lazysplits][%s] failed to parse protobuf from message queue!", thread_name_.c_str() ); 
		}
		pipe_to_cv_queue_->QueuePop();
	}

	should_read_queue_ = false;
}

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

} //namespace Lazysplits