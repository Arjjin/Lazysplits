//#define NOMINMAX

#include "SharedData\LzsSharedData.h"
#include "util\LzsImgProc.h"

namespace Lazysplits{
namespace SharedData{

LzsWatchColor::LzsWatchColor( const Proto::WatchInfo& watch_info, int watch_index, std::string watch_dir )
	:LzsWatchBase( watch_info, watch_index, watch_dir )
{
	auto color = watch_info.color();
	RGB_scalar_ = cv::Scalar( color.r(), color.g(), color.b(), 255.0 );
}

bool LzsWatchColor::FindWatch( const cv::Mat& BGR_frame, const SendableCalibrationProps& calib_props ){
	ValidateData( BGR_frame.cols, BGR_frame.rows, calib_props );
	if( IsGood() ){
		cv::Mat cropped_frame = BGR_frame(area_);
		return ImgProc::FindColor( cropped_frame, RGB_scalar_, 0.99 );
	}
	else{
		return false;
	}
}

bool LzsWatchColor::RemakeData(){
	return ( MakeArea() && CheckBounds() );
}

/*
bool LzsWatchColor::MakeArea(){
	int watch_x = watch_info_.area().loc().x();
	int watch_y = watch_info_.area().loc().y();
	int watch_width = watch_info_.area().size().x();
	int watch_height = watch_info_.area().size().y();
	//int watch_width = watch_info_.base_dimensions().x();
	//int watch_height = watch_info_.base_dimensions().y();

	float x_multiplier = (float)current_source_width_/watch_width;
	float y_multiplier = (float)current_source_height_/watch_height;

	if( current_calib_props_.is_enabled ){
		float calib_x_multiplier = ( (float)watch_width/ current_calib_props_.img_width ) * ( current_calib_props_.scale_x / 100.0F );
		float calib_y_multiplier = ( (float)watch_height/ current_calib_props_.img_height ) * ( current_calib_props_.scale_y / 100.0F );
		x_multiplier *= calib_x_multiplier;
		y_multiplier *= calib_y_multiplier;
	}

	//calibration offset is based off of source dimensions, so no need to adjust it
	int new_x = ( watch_x * x_multiplier ) + current_calib_props_.loc_x;
	int new_y = ( watch_y * y_multiplier ) + current_calib_props_.loc_y;
	//clamp out of bounds offsets
	area_.x = std::max<int>( 0, std::min<int>( new_x, current_source_width_ ) );
	area_.y = std::max<int>( 0, std::min<int>( new_y, current_source_height_ ) );
	area_.width = watch_width * x_multiplier;
	area_.height = watch_height * y_multiplier;

	if( area_.width <= 0 || area_.height <= 0 ){
		blog( LOG_DEBUG, "[lazysplits][SharedData] error making watch area for %s; width or height <= 0!", GetName().c_str() );
		return false;
	}
	return true;
}

bool LzsWatchColor::CheckBounds(){
	int area_x_neg_offset = std::max( 0, area_.x );
	int area_y_neg_offset = std::max( 0 , area_.y );
	cv::Rect source_rect( 0, 0, current_source_width_, current_source_height_ );

	//intersection of current watch area and source dimensions
	cv::Rect new_area = source_rect & area_;
	if( new_area.width <= 0 || new_area.height <=0 ){
		blog( LOG_DEBUG, "[lazysplits][SharedData] error checking watch bounds for %s; watch area does not intersect with source!", GetName().c_str() );
		return false;
	}
	area_ = new_area;

	return true;
}
*/

} //namepsace SharedData
} //namespace Lazysplits