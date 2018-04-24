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

		void OnSourceCreate( obs_data_t* source_settings, obs_source_t* context );
		obs_properties_t* GetSourceProps();
		void OnSourceUpdate( obs_data_t* source_settings );
		void OnSourceTick( float seconds );
		void OnSourceFilterVideo( obs_source_frame* frame );

		long GetFrameCount();
		
		obs_source_t* context_;
		LzsPipeServer pipe_server_;
		LzsCvThread cv_thread_;
		LzsFrameBuffer frame_buffer_;
	private :
		void InitProps( obs_source_t* context );

		long frame_count_;
		
		//OBS props stuff
		LzsObsPropList properties_;

		std::string prop_shared_data_dir;
		std::string prop_test_file;
};

} //namespace Lazysplits