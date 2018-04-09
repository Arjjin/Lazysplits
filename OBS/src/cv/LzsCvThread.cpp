#include "LzsCvThread.h"
#include "../util/LzsImgProc.h"

#include <opencv2\highgui.hpp>

#include <obs.h>

namespace Lazysplits{

LzsCvThread::LzsCvThread( LzsFrameBuffer* frame_buf, LzsMessageQueue<std::string>* cv_to_pipe_queue, LzsMessageQueue<std::string>* pipe_to_cv_queue )
	:LzsThread("CV thread")
{
	frame_buf_ = frame_buf;
	cv_to_pipe_queue_ = cv_to_pipe_queue;
	pipe_to_cv_queue_ = pipe_to_cv_queue;
	should_read_queue_ = false;
}

void LzsCvThread::OnSubjectNotify( std::string subject_name ){
	if( subject_name == "cv queue"){
		should_read_queue_ = true;
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
		//handle frames in frame buffer
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
	pipe_to_cv_queue_->DetachObserver(this);
	frame_buf_->DetachObserver(this);

	LzsThread::ThreadFuncCleanup();
}

void LzsCvThread::HandleMessages(){
	//do stuff

	if( pipe_to_cv_queue_->QueueIsEmpty() ){ should_read_queue_ = false; }
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
	else{ blog( LOG_WARNING, "[lazysplits][%s] frame buffer peek is bad", thread_name_ ); }
	frame_buf_->PopFrame();
}

} //namespace Lazysplits