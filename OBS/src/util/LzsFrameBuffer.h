#pragma once

#include "LzsObserver.h"

#include <obs.h>
#include <util\threading.h>
#include <opencv2\core.hpp>

#include <queue>
#include <memory>

namespace Lazysplits{

//TODO bool try methods for pushing/pulling data for thread safety
class LzsFrameBuffer : public LzsObservable{
	public :
		LzsFrameBuffer( int buf_max_count );
		~LzsFrameBuffer();

		void PushFrame( std::shared_ptr<cv::Mat> frame );
		std::shared_ptr<cv::Mat> const PeekFrame();
		void PopFrame();
		void Clear();

		int FrameCount();
	private :
		void PushFrameInternal( std::shared_ptr<cv::Mat> frame );
		std::shared_ptr<cv::Mat> const PeekFrameInternal();
		void PopFrameInternal();
		void LockMutex();
		void UnlockMutex();

		std::queue<std::shared_ptr<cv::Mat>> buf_;
		pthread_mutex_t buf_mutex_;
		const int buf_max_count_;
		int frame_count_;
};

} //namespace Lazysplits