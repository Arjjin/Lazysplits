#pragma once

#include <obs.h>
//#include <graphics\effect.h>
extern "C"{
#include <graphics\image-file.h>
}

#include <string>

namespace Lazysplits{

struct SendableCalibrationProps{
	bool is_enabled;
	int64_t img_width;
	int64_t img_height;
	int64_t loc_x;
	int64_t loc_y;
	double scale_x;
	double scale_y;
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