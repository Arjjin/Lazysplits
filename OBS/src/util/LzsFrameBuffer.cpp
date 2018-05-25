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

void LzsFrameBuffer::PushFrame( std::shared_ptr<cv::Mat> frame_mat, uint64_t timestamp ){
	LockMutex();
	LzsFrame new_frame;
	new_frame.frame_mat_ = frame_mat;
	new_frame.timestamp_ = timestamp;
	PushFrameInternal(new_frame);
	UnlockMutex();
}

LzsFrame LzsFrameBuffer::PeekFrame(){
	LzsFrame frame;
	LockMutex();
	frame = PeekFrameInternal();
	UnlockMutex();

	return frame;
}

void LzsFrameBuffer::PopFrame( ){
	LockMutex();
	PopFrameFrontInternal();
	UnlockMutex();
}

void LzsFrameBuffer::Clear(){
	LockMutex();
	buf_ = {};
	frame_count_ = 0;
	UnlockMutex();
}

int LzsFrameBuffer::FrameCount(){
	int frame_count;
	LockMutex();
	frame_count = frame_count_;
	UnlockMutex();

	return frame_count;
}

bool LzsFrameBuffer::Full(){
	int frame_count;
	LockMutex();
	frame_count = frame_count_;
	UnlockMutex();

	return frame_count >= buf_max_count_;
}

int LzsFrameBuffer::GetThrottleMod(){
	int frame_count;
	LockMutex();
	frame_count = frame_count_;
	UnlockMutex();

	if( frame_count >= 15 ){
		return 2;
	}
	else if( frame_count >= 25 ){
		return 4;
	}

	return 1;
}

void LzsFrameBuffer::PushFrameInternal( LzsFrame frame ){
	//pop a frame if our buffer is at max capacity
	if( frame_count_ == buf_max_count_ ){ PopFrameBackInternal(); }
	buf_.push_back(frame);
	frame_count_++;
	NotifyAll();
}

LzsFrame LzsFrameBuffer::PeekFrameInternal(){
	return buf_.front();
}

void LzsFrameBuffer::PopFrameFrontInternal(){
	buf_.pop_front();
	frame_count_--;
}

void LzsFrameBuffer::PopFrameBackInternal(){
	buf_.pop_back();
	frame_count_--;
}

void LzsFrameBuffer::LockMutex(){ pthread_mutex_lock(&buf_mutex_); }

void LzsFrameBuffer::UnlockMutex(){ pthread_mutex_unlock(&buf_mutex_); }

} //namespace Lazysplits