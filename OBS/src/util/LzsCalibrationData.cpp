#include "LzsCalibrationData.h"

#include <obs-module.h>
#include <graphics\effect.h>
#include <graphics\vec2.h>

#include <filesystem>

namespace filesys = std::experimental::filesystem;

namespace Lazysplits{

LzsCalibrationData::LzsCalibrationData( const std::string& obs_module_path ){
	img_ = static_cast<gs_image_file_t*>( bzalloc( sizeof(gs_image_file_t) ) );
	//effect_ = static_cast<gs_effect_t*>( bzalloc( sizeof(gs_effect_t) ) );

	is_adjusting_ = false;
	calib_opacity_ = 1.0F;
	img_path_ = "";
	current_sendable_props_.is_enabled = false;
	current_sendable_props_.img_width = 0;
	current_sendable_props_.img_height = 0;
	current_sendable_props_.loc_x = 0;
	current_sendable_props_.loc_y = 0;
	current_sendable_props_.scale_x = 100.0F;
	current_sendable_props_.scale_y = 100.0F;
	current_sendable_props_.use_nn_interp = false;

	//load shader
	filesys::path effect_path = obs_module_path;
	effect_path /= "calibration.effect";
	if( filesys::exists(effect_path) ){
		char* effect_error_string;

		obs_enter_graphics();
		effect_ = gs_effect_create_from_file( effect_path.string().c_str(), &effect_error_string );
		obs_leave_graphics();

		if(!effect_){ blog( LOG_ERROR, "[Lazysplits][Calibration] failed to create calibration effect; %s", effect_error_string ); }
		else{ blog( LOG_DEBUG, "[Lazysplits][Calibration] calibration effect created" ); }
	}
	else{ blog( LOG_ERROR, "[Lazysplits][Calibration] failed to create calibration effect; file not found at %s!", effect_path.string().c_str() ); }
}

LzsCalibrationData::~LzsCalibrationData(){
	obs_enter_graphics();
	gs_effect_destroy(effect_);
	gs_image_file_free(img_);
	obs_leave_graphics();

	bfree(img_);
}

void LzsCalibrationData::UpdateImg(){
	filesys::path new_path = img_path_;
	if( filesys::exists(new_path) ){
		obs_enter_graphics();
		gs_image_file_free(img_);
		gs_image_file_init( img_, new_path.string().c_str() );
		if(img_){
			current_sendable_props_.img_width = img_->cx;
			current_sendable_props_.img_height = img_->cy;
			gs_image_file_init_texture(img_);
		}
		obs_leave_graphics();
		
		blog( LOG_INFO, "[Lazysplits][Calibration] calibration image successfully updated (%s)", img_path_.c_str() );
	}
}

bool LzsCalibrationData::ImgValid(){
	return ( img_ && img_->texture && ( img_->cx > 0 ) && ( img_->cy > 0 ) );
}

void LzsCalibrationData::SetDimensions( int64_t loc_x, int64_t loc_y, double scale_x, double scale_y ){
	current_sendable_props_.loc_x = loc_x;
	current_sendable_props_.loc_y = loc_y;
	current_sendable_props_.scale_x = scale_x;
	current_sendable_props_.scale_y = scale_y;
}

bool LzsCalibrationData::ShouldResend(){
	//ugly
	if( ImgValid() ){
		if( current_sendable_props_.is_enabled != last_sent_props_.is_enabled ){
			return true;
		}
		//only resend calib info changes if calibration is enabled
		else if( current_sendable_props_.is_enabled && 
				 (
					current_sendable_props_.img_width     != last_sent_props_.img_width     ||
					current_sendable_props_.img_height    != last_sent_props_.img_height    ||
					current_sendable_props_.loc_x	      != last_sent_props_.loc_x	        ||	
					current_sendable_props_.loc_y	      != last_sent_props_.loc_y	        ||
					current_sendable_props_.scale_x	      != last_sent_props_.scale_x	    ||
					current_sendable_props_.scale_y	      != last_sent_props_.scale_y	    ||
					current_sendable_props_.use_nn_interp != last_sent_props_.use_nn_interp
				 )
		){
			return true;
		}
	}
	return false;
}

const SendableCalibrationProps& LzsCalibrationData::SendData(){
	last_sent_props_ = current_sendable_props_;
	return current_sendable_props_;
}

void LzsCalibrationData::Render( obs_source_t* context ){
	if( img_ && img_->texture && effect_ ){
		if ( obs_source_process_filter_begin( context, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING) ){
			
			gs_eparam_t *param;

			param = gs_effect_get_param_by_name( effect_, "calib_image" );
			gs_effect_set_texture( param, img_->texture );

			vec2 source_dimensions_vec { obs_source_get_width(context), obs_source_get_height(context) };
			param = gs_effect_get_param_by_name( effect_, "source_dimensions" );
			gs_effect_set_vec2( param, &source_dimensions_vec );

			vec2 calib_dimensions_vec { img_->cx, img_->cy };
			param = gs_effect_get_param_by_name( effect_, "calib_dimensions" );
			gs_effect_set_vec2( param, &calib_dimensions_vec );

			param = gs_effect_get_param_by_name( effect_, "calib_opacity" );
			gs_effect_set_float( param, static_cast<float>(calib_opacity_) );

			vec2 calib_loc_vec { current_sendable_props_.loc_x, current_sendable_props_.loc_y };
			param = gs_effect_get_param_by_name( effect_, "calib_loc" );
			gs_effect_set_vec2( param, &calib_loc_vec );

			vec2 calib_scale_vec { current_sendable_props_.scale_x, current_sendable_props_.scale_y };
			param = gs_effect_get_param_by_name( effect_, "calib_scale_percent" );
			gs_effect_set_vec2( param, &calib_scale_vec );
			
			obs_source_process_filter_end( context, effect_, 0, 0 );
		}
		else{
			blog( LOG_DEBUG, "[Lazysplits][Calibration] failed to begin video render!" );
		}
	}
	else{
		/*
		blog( LOG_DEBUG, "[Lazysplits][Calibration] Skipping video render; calib image : %s, calib image->texture : %s, calib effects : %s",
			(img_) ? "good" : "null",
			( img_ && img_->texture ) ? "good" : "null",
			(effect_) ? "good" : "null" 
		);
		*/
		obs_source_skip_video_filter(context);
	}
}

} //namespace Lazysplits