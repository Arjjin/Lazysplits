#pragma once

#include "LzsObserver.h"

#include <obs.h>
#include <util\threading.h>
#include <opencv2\core.hpp>

#include <queue>
#include <memory>

namespace Lazysplits{

struct LzsFrame{
	uint64_t timestamp_;
	std::shared_ptr<cv::Mat> frame_mat_;
};

//TODO bool try methods for pushing/pulling data for thread safety
class LzsFrameBuffer : public LzsObservable{
	public :
		LzsFrameBuffer( int buf_max_count );
		~LzsFrameBuffer();

		void PushFrame( std::shared_ptr<cv::Mat> frame_mat, uint64_t timestamp );
		LzsFrame PeekFrame();
		void PopFrame();
		void Clear();

		int FrameCount();
	private :
		void PushFrameInternal( LzsFrame frame );
		LzsFrame PeekFrameInternal();
		void PopFrameInternal();
		void LockMutex();
		void UnlockMutex();

		std::queue<LzsFrame> buf_;
		pthread_mutex_t buf_mutex_;
		const int buf_max_count_;
		int frame_count_;
};

} //namespace Lazysplits