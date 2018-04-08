#include "LzsFrameBuffer.h"

namespace Lazysplits{

LzsFrameBuffer::LzsFrameBuffer( obs_source_t* frame_source, int buf_max_count ){
	frame_source_ = frame_source;
	buf_max_count_ = buf_max_count;
	frame_count_ = 0;
	circlebuf_init(&buf_);
	pthread_mutex_init( &buf_mutex_, NULL );
}

LzsFrameBuffer::~LzsFrameBuffer(){
	FreeFrameBuffer();
	pthread_mutex_destroy(&buf_mutex_);
}

void LzsFrameBuffer::PushFrame( obs_source_frame* frame ){
	LockMutex();
	PushFrameInternal(frame);
	UnlockMutex();
}

obs_source_frame* const LzsFrameBuffer::PeekFrame(){
	obs_source_frame* frame;
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

void LzsFrameBuffer::PushFrameInternal( obs_source_frame* frame ){
	if(frame){
		//pop a frame if our buffer is at max capacity
		if( frame_count_ == buf_max_count_ ){ PopFrameInternal(); }
		circlebuf_push_back( &buf_, frame, sizeof(obs_source_frame*) );
		frame_count_++;
	}
}

obs_source_frame* const LzsFrameBuffer::PeekFrameInternal(){
	obs_source_frame* frame;
	circlebuf_peek_front( &buf_, &frame, sizeof(obs_source_frame*) );

	return frame;
}

void LzsFrameBuffer::PopFrameInternal(){
	obs_source_frame* frame;
	circlebuf_pop_front( &buf_, &frame, sizeof(obs_source_frame*) );
	if(frame){ obs_source_release_frame( frame_source_, frame ); }
	frame_count_--;
}

void LzsFrameBuffer::FreeFrameBuffer(){
	blog( LOG_DEBUG, "[Lazysplits] freeing %i frames from buffer", frame_count_ );
	while( frame_count_ > 0 ){ PopFrameInternal(); }
	circlebuf_free(&buf_);
}

void LzsFrameBuffer::LockMutex(){ pthread_mutex_lock(&buf_mutex_); }

void LzsFrameBuffer::UnlockMutex(){ pthread_mutex_unlock(&buf_mutex_); }

} //namespace Lazysplits