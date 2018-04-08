#pragma once

#include <obs-module.h>

#include "pipe\LzsPipeServer.h"
#include "pipe\LzsMessageQueue.h"
#include ".\util\LzsFrameBuffer.h"

#include <string>

namespace Lazysplits{

class LzsSourceData{
	public :
		LzsSourceData( obs_source_t* context );
		~LzsSourceData();
		
		obs_source_t* context_;
		LzsPipeServer pipe_server_;
		LzsFrameBuffer frame_buffer_;
	private :
		LzsMessageQueue<std::string> cv_to_pipe_queue_;
		LzsMessageQueue<std::string> pipe_to_cv_queue_;
};

} //namespace Lazysplits