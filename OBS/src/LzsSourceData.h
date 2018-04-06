#pragma once

#include <obs-module.h>

#include "pipe\LzsPipeServer.h"
#include "pipe\LzsMessageQueue.h"

#include <string>

namespace Lazysplits{

class LzsSourceData{
	public :
		LzsSourceData( obs_source_t* context );
		~LzsSourceData();

		LzsPipeServer pipe_server_;
	private :
		obs_source_t* context_;
		LzsMessageQueue<std::string> cv_to_pipe_queue_;
		LzsMessageQueue<std::string> pipe_to_cv_queue_;
};

} //namespace Lazysplits