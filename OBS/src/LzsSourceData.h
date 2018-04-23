#pragma once

#include <string>
#include <vector>
#include <memory>

#include <obs-module.h>

#include "LzsObsProps.h"
#include "cv\LzsCvThread.h"
#include "util\LzsFrameBuffer.h"
#include "pipe\LzsPipeServer.h"
#include "pipe\LzsMessageQueue.h"

namespace Lazysplits{

class LzsSourceData{
	public :
		LzsSourceData( obs_source_t* context );
		~LzsSourceData();

		void FrameTick();
		long FrameCount();
		
		obs_source_t* context_;
		LzsPipeServer pipe_server_;
		LzsCvThread cv_thread_;
		LzsFrameBuffer frame_buffer_;

		std::string shared_dir_root_;

		bool test_bool;
		int64_t test_int;
		LzsObsPropList properties_;
	private :
		long frame_count_;

		LzsMessageQueue<std::string> cv_to_pipe_queue_;
		LzsMessageQueue<std::string> pipe_to_cv_queue_;
};

} //namespace Lazysplits