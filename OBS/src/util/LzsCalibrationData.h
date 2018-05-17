#pragma once

#include <obs.h>
//#include <graphics\effect.h>
extern "C"{
#include <graphics\image-file.h>
}

#include <string>

namespace Lazysplits{

struct SendableCalibrationProps{
	bool is_enabled = false;
	int64_t img_width = 0;
	int64_t img_height = 0;
	int64_t loc_x = 0;
	int64_t loc_y = 0;
	double scale_x = 100.0F;
	double scale_y  = 100.0F;
};

class LzsCalibrationData{
	public :
		LzsCalibrationData( const std::string& obs_module_path );
		~LzsCalibrationData();
		
		//bool IsCalibration();
		void UpdateImg();
		bool ImgValid();
		void SetDimensions( int64_t loc_x, int64_t loc_y, double scale_x, double scale_y );
		bool ShouldResend();
		const SendableCalibrationProps& SendData();

		void Render( obs_source_t* context );
		
		bool is_adjusting_;
		std::string img_path_;
		double calib_opacity_;
		SendableCalibrationProps current_sendable_props_;
	private :
		SendableCalibrationProps last_sent_props_;


		gs_image_file_t* img_;
		gs_effect_t* effect_;
};

} //namespace Lazysplits