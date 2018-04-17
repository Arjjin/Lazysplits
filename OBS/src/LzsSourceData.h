#pragma once

#include <obs-module.h>

#include "util\LzsFrameBuffer.h"
#include "cv\LzsCvThread.h"
#include "pipe\LzsPipeServer.h"
#include "pipe\LzsMessageQueue.h"

#include <string>

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
	private :
		long frame_count_;

		LzsMessageQueue<std::string> cv_to_pipe_queue_;
		LzsMessageQueue<std::string> pipe_to_cv_queue_;
};

} //namespace Lazysplits