#include "LzsFrameBuffer.h"

namespace Lazysplits{

LzsFrameBuffer::LzsFrameBuffer( int buf_max_count )
	:LzsObservable("frame buf"),
	 buf_max_count_(buf_max_count)
{
	frame_count_ = 0;

	pthread_mutex_init( &buf_mutex_, NULL );
}

LzsFrameBuffer::~LzsFrameBuffer(){
	pthread_mutex_destroy(&buf_mutex_);
}

void LzsFrameBuffer::PushFrame( std::shared_ptr<cv::Mat> frame ){
	LockMutex();
	PushFrameInternal(frame);
	UnlockMutex();
}

std::shared_ptr<cv::Mat> const LzsFrameBuffer::PeekFrame(){
	std::shared_ptr<cv::Mat> frame;
	LockMutex();
	frame = PeekFrameInternal();
	UnlockMutex();

	return frame;
}

void LzsFrameBuffer::PopFrame( ){
	LockMutex();
	PopFrameInternal();
	UnlockMutex();
}

void LzsFrameBuffer::Clear(){
	LockMutex();
	buf_ = {};
	frame_count_ = 0;
	UnlockMutex();
}

int LzsFrameBuffer::FrameCount(){
	int frame_count = 0;
	LockMutex();
	frame_count = frame_count_;
	UnlockMutex();

	return frame_count;
}

void LzsFrameBuffer::PushFrameInternal( std::shared_ptr<cv::Mat> frame ){
	if(frame){
		//pop a frame if our buffer is at max capacity
		if( frame_count_ == buf_max_count_ ){ PopFrameInternal(); }
		buf_.push(frame);
		frame_count_++;
		NotifyAll();
	}
}

std::shared_ptr<cv::Mat> const LzsFrameBuffer::PeekFrameInternal(){
	return buf_.front();
}

void LzsFrameBuffer::PopFrameInternal(){
	buf_.pop();
	frame_count_--;
}

void LzsFrameBuffer::LockMutex(){ pthread_mutex_lock(&buf_mutex_); }

void LzsFrameBuffer::UnlockMutex(){ pthread_mutex_unlock(&buf_mutex_); }

} //namespace Lazysplits