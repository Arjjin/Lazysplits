#include "SharedData\LzsSharedData.h"
#include "cv\LzsCvConstants.h"

#include <opencv2\highgui.hpp>

#include <filesystem>

namespace filesys = std::experimental::filesystem;

namespace Lazysplits{
namespace SharedData{

LzsWatch::LzsWatch( const Proto::WatchInfo& watch_info, int watch_index, std::string watch_dir ){
	watch_info_ = watch_info;
	index_ = watch_index;
	watch_dir_ = watch_dir;

	img_ = LZS_MAT_UNITIALIZED;
}

const std::string& LzsWatch::GetName(){ return watch_info_.name(); }

Proto::WatchType LzsWatch::GetType(){ return watch_info_.type(); }

int LzsWatch::GetIndex(){ return index_; }

const cv::Mat& LzsWatch::GetImage( const SendableCalibrationProps& calib_props ){
	//if we have different calibration data, remake our image
	if( IsNewCalibProps(calib_props) ){
		current_calib_props_ = calib_props;
		MakeImage();
	}
	return img_;
}

void LzsWatch::MakeImage(){
	filesys::path img_path = watch_dir_;
	img_path /= watch_info_.img_relative_path();

	bool img_error = false;
	try{
		//try to read the image
		img_ = cv::imread( img_path.string(), cv::IMREAD_UNCHANGED );
		auto watch_size =  watch_info_.area().size();
		if( img_.cols <= 0 || img_.rows <= 0 ){
			blog( LOG_ERROR, "[lazysplits][SharedData] error reading watch image from file; width or height is <= 0!" );
			img_error = true;
		}
		else{

		}
	}
	catch( cv::Exception cve ){
		blog( LOG_ERROR, "[lazysplits][SharedData] error reading watch image from file; %s", cve.msg.c_str() );
		img_error = true;
	}
	if( img_error ){ img_ = LZS_MAT_ERROR; }
}

bool LzsWatch::IsNewCalibProps( const SendableCalibrationProps& calib_props ){
	return current_calib_props_.is_enabled != calib_props.is_enabled ||
		   current_calib_props_.img_width  != calib_props.img_width  ||
		   current_calib_props_.img_height != calib_props.img_height ||
		   current_calib_props_.loc_x	   != calib_props.loc_x	     ||
		   current_calib_props_.loc_y	   != calib_props.loc_y	     ||
		   current_calib_props_.scale_x	   != calib_props.scale_x	 ||
		   current_calib_props_.scale_y	   != calib_props.scale_y    ;
}

} //namepsace SharedData
} //namespace Lazysplits