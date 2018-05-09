#pragma once

#include <obs-module.h>

#include "LzsObsProps.h"
#include "cv\LzsCvThread.h"
#include "util\LzsFrameBuffer.h"
#include "pipe\LzsPipeServer.h"
#include "pipe\LzsMessageQueue.h"
#include "SharedData\LzsSharedData.h"
#include "util\LzsCalibrationData.h"

#include <string>
#include <vector>
#include <memory>

namespace Lazysplits{

class LzsSourceData{
	public :
		LzsSourceData( obs_source_t* context, const std::string& module_data_path );
		~LzsSourceData();

		void OnSourceCreate( obs_data_t* settings, obs_source_t* context );
		obs_properties_t* GetSourceProps();
		void OnSourceUpdate( obs_data_t* settings );
		void OnSourceTick( float seconds );
		void OnSourceRenderVideo( gs_effect_t* effect );
		void OnSourceFilterVideo( obs_source_frame* frame );
		void OnSourceSave( obs_data_t* settings );

		long GetFrameCount();

		//OBS prop callback
		static bool PropCalibEnabledModified( obs_properties_t *props, obs_property_t *p, obs_data_t *settings );
		
		obs_source_t* context_;
		LzsPipeServer pipe_server_;
		LzsCvThread cv_thread_;
		LzsFrameBuffer frame_buffer_;
	private :
		void InitProps( obs_source_t* context );

		long frame_count_;
		std::string module_data_path_;
		
		LzsObsPropsList properties_;
		std::string prop_shared_data_dir_;
		LzsCalibrationData calib_data_;
};

} //namespace Lazysplits