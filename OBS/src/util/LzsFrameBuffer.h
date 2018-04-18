#pragma once

#include "LzsObserver.h"

#include <obs.h>
#include <util\circlebuf.h>
#include <util\threading.h>

namespace Lazysplits{

//wrapping a dynamic sized buffer to have a fixed capacity
//I'm a genius
class LzsFrameBuffer : public LzsObservable{
	public :
		LzsFrameBuffer( int buf_max_count );
		~LzsFrameBuffer();

		void PushFrame( obs_source_frame* frame );
		obs_source_frame* const PeekFrame();
		void PopFrame();

		int FrameCount();
	private :
		void PushFrameInternal( obs_source_frame* frame );
		obs_source_frame* const PeekFrameInternal();
		void PopFrameInternal();
		void FreeFrameBuffer();
		void LockMutex();
		void UnlockMutex();

		circlebuf buf_;
		pthread_mutex_t buf_mutex_;
		int buf_max_count_;
		int frame_count_;
};

} //namespace Lazysplits