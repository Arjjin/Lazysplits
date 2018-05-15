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

	SetUnitialized();
}

bool LzsWatchBase::IsGood(){ return status_ == LZS_WATCH_GOOD; }

const std::string& LzsWatchBase::GetName(){ return watch_info_.name(); }

Proto::WatchType LzsWatchBase::GetType(){ return watch_info_.type(); }

int LzsWatchBase::GetIndex(){ return index_; }

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

bool LzsWatchBase::CheckBounds(){
	int area_x_neg_offset = std::max( 0, area_.x );
	int area_y_neg_offset = std::max( 0 , area_.y );
	cv::Rect source_rect( 0, 0, current_source_width_, current_source_height_ );

	//intersection of current watch area and source dimensions
	cv::Rect area_ = source_rect & area_;
	if( area_.width <= 0 || area_.height <=0 ){
		blog( LOG_DEBUG, "[lazysplits][SharedData] error checking watch bounds for %s; watch area does not intersect with source!", GetName().c_str() );
		return false;
	}
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