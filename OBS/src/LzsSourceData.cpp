#include "LzsSourceData.h"

#include "SharedData\LzsSharedData.h"

//DEL
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

#include <chrono>

namespace Lazysplits{

LzsSourceData::LzsSourceData( obs_source_t* context, const std::string& module_data_path )
	:context_(context),
	 frame_buffer_(30),
	 pipe_server_( "lazysplits_pipe", 8192 ),
	 cv_thread_( &frame_buffer_, context_ ),
	 calib_data_(module_data_path)
{
	frame_count_ = 0;
	last_cap_ = 0;

	//pass thread refrences to each other
	pipe_server_.AssignCvThread(&cv_thread_);
	cv_thread_.AssignPipe(&pipe_server_);

	//set OBS property defaults
	prop_shared_data_dir_ = "";
	InitProps(context);

	//create threads
	pipe_server_.ThreadCreate();
	cv_thread_.ThreadCreate();
}

LzsSourceData::~LzsSourceData(){
	if( pipe_server_.IsThreadLive() ){
		pipe_server_.ThreadTerminate();
		pipe_server_.ThreadJoin();
	}
	if( cv_thread_.IsThreadLive() ){
		cv_thread_.ThreadTerminate();
		cv_thread_.ThreadJoin();
	}

	obs_enter_graphics();
	if(texrender_){ gs_texrender_destroy(texrender_); }
	if(stagesurface_){ gs_stagesurface_destroy(stagesurface_); }
	obs_leave_graphics();
}

void LzsSourceData::InitProps( obs_source_t* context ){
	properties_.AddPath( &prop_shared_data_dir_, "shared_data_path", "Shared data directory", OBS_PATH_DIRECTORY, "", "" );
	properties_.AddBool( &calib_data_.current_sendable_props_.is_enabled, "calib_enable", "Enable calibration" );
	properties_.AddBool( &calib_data_.is_adjusting_, "calib_adjust", "Set calibration", PropCalibEnabledModified );
	properties_.AddPath( &calib_data_.img_path_, "calib_img_path", "Calibration image", OBS_PATH_FILE, "PNG files (*.PNG)", prop_shared_data_dir_ );
	properties_.AddFloat( &calib_data_.calib_opacity_, "calib_opacity", "Calibration image opacity", 0.0F, 1.0F, 0.01 );
	properties_.AddInt( &calib_data_.current_sendable_props_.loc_x, "calib_loc_x", "Calibration X", -200, 200, 1 );
	properties_.AddInt( &calib_data_.current_sendable_props_.loc_y, "calib_loc_y", "Calibration Y", -200, 200, 1 );
	properties_.AddFloat( &calib_data_.current_sendable_props_.scale_x, "calib_scale_x", "Calibration width %", 10.0F, 400.0F, 0.1F );
	properties_.AddFloat( &calib_data_.current_sendable_props_.scale_y, "calib_scale_y", "Calibration height %", 10.0F, 400.0F, 0.1F );
	
	obs_data_t* source_settings = obs_source_get_settings(context);
	properties_.SetPropertyDefaults(source_settings);
	obs_data_release(source_settings);
}

void LzsSourceData::OnSourceCreate( obs_data_t* settings, obs_source_t* context ){
	properties_.UpdateProperties(settings);
	cv_thread_.MsgSetSharedDataPath(prop_shared_data_dir_);
	calib_data_.UpdateImg();
	if( calib_data_.ShouldResend() ){
		cv_thread_.MsgCalibData( calib_data_.SendData() );
	}
}

obs_properties_t* LzsSourceData::GetSourceProps(){
	obs_properties_t *props = obs_properties_create();
	properties_.AddProperties(props);

	return props;

}

void LzsSourceData::OnSourceUpdate( obs_data_t *settings ){
	std::string last_shared_data_dir = prop_shared_data_dir_;
	std::string  last_calib_img_path = calib_data_.img_path_;

	properties_.UpdateProperties(settings);

	if( last_shared_data_dir != prop_shared_data_dir_ ){
		cv_thread_.MsgSetSharedDataPath(prop_shared_data_dir_);
	}
	if( last_calib_img_path != calib_data_.img_path_ ){
		calib_data_.UpdateImg();
	}
}

void LzsSourceData::OnSourceTick( float seconds ){
	frame_count_++; 
}

void LzsSourceData::OnSourceRenderVideo( gs_effect_t* effect ){
    obs_source_t *target = obs_filter_get_target(context_);
    obs_source_t *parent = obs_filter_get_parent(context_);
	
	//if( frame_count_ % 4 == 0 && cv_thread_.IsTargets() ){
	if( cv_thread_.IsTargets() ){
		GrabRenderFrame( target, parent );
	}

	if( calib_data_.is_adjusting_ ){
		calib_data_.Render(context_);
	}
	else if(target){
		obs_source_video_render(target);
	}
	else if(parent){
		obs_source_skip_video_filter(context_);
	}
}

void LzsSourceData::GrabRenderFrame( obs_source_t* target, obs_source_t* parent ){
	if( !target || !parent || last_cap_ == frame_count_ ){ return; }
	
	auto epoch_ms = std::chrono::time_point_cast<std::chrono::milliseconds>( std::chrono::system_clock::now() ).time_since_epoch();
	uint32_t target_width = obs_source_get_base_width(target);
	uint32_t target_height = obs_source_get_base_height(target);

	//no video data
	if( target_width <= 1 || target_height <= 1 ){ return; }

	if(!texrender_){ texrender_ = gs_texrender_create( GS_RGBA, GS_ZS_NONE ); }
	else{ gs_texrender_reset(texrender_); }

    if (!stagesurface_) {
        stagesurface_ = gs_stagesurface_create( target_width, target_height, GS_RGBA );
    }
	else if( gs_stagesurface_get_width(stagesurface_) != target_width || gs_stagesurface_get_height(stagesurface_) != target_height ){
		gs_stagesurface_destroy(stagesurface_);
        stagesurface_ = gs_stagesurface_create( target_width, target_height, GS_RGBA );
	}
	if( gs_texrender_begin( texrender_, target_width, target_height ) ){
		//clip our source image out of total output dimensions?
        gs_ortho(0.0f, (float)target_width, 0.0f, (float)target_height, 0.0F, 1.0F );
        obs_source_video_render(target);
        gs_texrender_end(texrender_);

		gs_stage_texture( stagesurface_, gs_texrender_get_texture(texrender_) );

		uint8_t* tex_ptr;
		uint32_t linesize = 0;
		if( gs_stagesurface_map( stagesurface_, &tex_ptr, &linesize ) ){
			std::shared_ptr<cv::Mat> mat_ptr = std::make_shared<cv::Mat>( target_height, target_width, CV_8UC4, tex_ptr, linesize );
			gs_stagesurface_unmap(stagesurface_);
			
			frame_buffer_.PushFrame( mat_ptr, epoch_ms.count() );
		}
	}

	last_cap_ = frame_count_;
}

void LzsSourceData::OnSourceSave( obs_data_t* settings ){
	blog( LOG_DEBUG, "[Lazysplits] lzs_source_save");
	
	if( calib_data_.ShouldResend() ){
		cv_thread_.MsgCalibData( calib_data_.SendData() );
	}
}

const long LzsSourceData::GetFrameCount(){ return frame_count_; }

bool LzsSourceData::PropCalibEnabledModified( obs_properties_t *props, obs_property_t *p, obs_data_t *settings ){
	bool calib_active = obs_data_get_bool( settings, "calib_adjust" );
	
	obs_property_set_visible( obs_properties_get( props, "calib_img_path" ), calib_active );
	obs_property_set_visible( obs_properties_get( props, "calib_opacity" ), calib_active );
	obs_property_set_visible( obs_properties_get( props, "calib_loc_x" ), calib_active );
	obs_property_set_visible( obs_properties_get( props, "calib_loc_y" ), calib_active );
	obs_property_set_visible( obs_properties_get( props, "calib_scale_x" ), calib_active );
	obs_property_set_visible( obs_properties_get( props, "calib_scale_y" ), calib_active );

	return true;
}

} //namespace Lazysplits