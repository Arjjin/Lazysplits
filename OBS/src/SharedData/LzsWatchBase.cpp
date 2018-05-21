//#define NOMINMAX

#include "SharedData\LzsSharedData.h"

#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

#include <algorithm>

namespace Lazysplits{
namespace SharedData{

LzsWatchBase::LzsWatchBase( const Proto::WatchInfo& watch_info, int watch_index, std::string watch_dir ){
	watch_info_ = watch_info;
	index_ = watch_index;
	watch_dir_ = watch_dir;
	current_source_width_ = 0;
	current_source_height_ = 0;

	SetUnitialized();
}

bool LzsWatchBase::IsGood(){ return status_ == LZS_WATCH_GOOD; }

const std::string& LzsWatchBase::GetName(){ return watch_info_.name(); }

Proto::WatchType LzsWatchBase::GetType(){ return watch_info_.type(); }

int LzsWatchBase::GetIndex(){ return index_; }

bool LzsWatchBase::FindWatch( const cv::Mat& BGR_frame, const SendableCalibrationProps& calib_props  ){
	ValidateData( BGR_frame.cols, BGR_frame.rows, calib_props );
	if( IsGood() ){
		return CvLogic( BGR_frame );
	}
	else{
		return false;
	}
}

void LzsWatchBase::SetError(){ status_ = LZS_WATCH_ERROR; }

void LzsWatchBase::SetUnitialized(){ status_ = LZS_WATCH_UNITIALIZED; }

void LzsWatchBase::SetGood(){ status_ = LZS_WATCH_GOOD; }

void LzsWatchBase::ValidateData( int source_width, int source_height, const SendableCalibrationProps& calib_props ){
	if( ShouldRemakeData( source_width, source_height, calib_props ) ){
		if( RemakeData() ){
			SetGood();
		}
		else{
			SetError();
		}
	}
}

bool LzsWatchBase::ShouldRemakeData( int source_width, int source_height, const SendableCalibrationProps& calib_props ){
	bool is_new_data = false;
	if( source_width != current_source_width_ ){
		current_source_width_ = source_width;
		is_new_data = true;
	}
	if( source_height != current_source_height_ ){
		current_source_height_ = source_height;
		is_new_data = true;
	}
	if( IsDiffCalibProps(calib_props) ){
		current_calib_props_ = calib_props;
		is_new_data = true;
	}
	return is_new_data;
}

bool LzsWatchBase::MakeArea(){
	int additional_padding = watch_info_.additional_area_padding();
	int area_x = watch_info_.area().loc().x() - additional_padding;
	int area_y = watch_info_.area().loc().y() - additional_padding;
	int area_width = watch_info_.area().size().x() + ( additional_padding * 2 );
	int area_height = watch_info_.area().size().y() + ( additional_padding * 2 );
	int watch_base_width = watch_info_.base_dimensions().x();
	int watch_base_height = watch_info_.base_dimensions().y();

	float x_multiplier = (float)current_source_width_/watch_base_width;
	float y_multiplier = (float)current_source_height_/watch_base_height;

	if( current_calib_props_.is_enabled ){
		float calib_x_multiplier = ( (float)current_calib_props_.img_width/watch_base_width ) * ( current_calib_props_.scale_x / 100.0F );
		float calib_y_multiplier = ( (float)current_calib_props_.img_height/watch_base_height ) * ( current_calib_props_.scale_y / 100.0F );
		x_multiplier *= calib_x_multiplier;
		y_multiplier *= calib_y_multiplier;
	}

	//calibration loc (offset) is based off of source dimensions, so no need to adjust it
	int new_x = ( area_x * x_multiplier ) + current_calib_props_.loc_x;
	int new_y = ( area_y * y_multiplier ) + current_calib_props_.loc_y;
	//clamp out of bounds offsets
	//area_.x = std::max<int>( 0, std::min<int>( new_x, current_source_width_ ) );
	//area_.y = std::max<int>( 0, std::min<int>( new_y, current_source_height_ ) );
	//or not? should be handled properly by CheckBounds()
	area_.x = new_x;
	area_.y = new_y;
	area_.width = area_width * x_multiplier;
	area_.height = area_height * y_multiplier;

	if( area_.width <= 0 || area_.height <= 0 ){
		blog( LOG_DEBUG, "[lazysplits][SharedData] error making watch area for %s; width or height <= 0!", GetName().c_str() );
		return false;
	}
	return true;
}

bool LzsWatchBase::CheckBounds(){
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

bool LzsWatchBase::IsDiffCalibProps( const SendableCalibrationProps& calib_props ){
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