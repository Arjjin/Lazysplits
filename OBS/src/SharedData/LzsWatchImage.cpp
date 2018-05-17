//#define NOMINMAX

#include "SharedData\LzsSharedData.h"
#include "util\LzsImgProc.h"

#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

#include <algorithm>
#include <filesystem>

namespace filesys = std::experimental::filesystem;

namespace Lazysplits{
namespace SharedData{

/* LzsWatchImageBase */

LzsWatchImageBase::LzsWatchImageBase( const Proto::WatchInfo& watch_info, int watch_index, std::string watch_dir )
	:LzsWatchBase( watch_info, watch_index, watch_dir)
{}

bool LzsWatchImageBase::MakeImage(){
	filesys::path img_path = watch_dir_;
	img_path /= watch_info_.img_relative_path();

	try{
		cv::Mat new_img = cv::imread( img_path.string(), cv::IMREAD_UNCHANGED );
		if( new_img.cols <= 0 || new_img.rows <= 0 ){
			blog( LOG_ERROR, "[lazysplits][SharedData] error reading watch image (%s) for watch %s; width or height is <= 0!", img_path.string().c_str(), GetName().c_str() );
			return false;
		}

		/* general resizing to source/calibration */
			
		//int watch_width = watch_info_.base_dimensions().x();
		//int watch_height = watch_info_.base_dimensions().y();
		int watch_width = watch_info_.area().size().x();
		int watch_height = watch_info_.area().size().y();
		float x_multiplier = (float)current_source_width_/watch_width;
		float y_multiplier = (float)current_source_height_/watch_height;

		if( current_calib_props_.is_enabled ){
			float calib_x_multiplier = ( (float)watch_width/ current_calib_props_.img_width ) * ( current_calib_props_.scale_x / 100.0F );
			float calib_y_multiplier = ( (float)watch_height/ current_calib_props_.img_height ) * ( current_calib_props_.scale_y / 100.0F );
			x_multiplier *= calib_x_multiplier;
			y_multiplier *= calib_y_multiplier;
		}
		int new_width = new_img.cols * x_multiplier;
		int new_height = new_img.rows * y_multiplier;

		cv::Mat new_resized_img;
		cv::resize( new_img, new_resized_img, cv::Size( new_width, new_height ) );

		/* moving BGRA mat into BGR and mask mats */

		img_BGR_ = cv::Mat( new_resized_img.rows, new_resized_img.cols, CV_8UC3 );
		img_mask_ = cv::Mat( new_resized_img.rows, new_resized_img.cols, CV_8UC3 );
		cv::Mat mix_destination[] = { img_BGR_, img_mask_ };
		int mix_pairing[] = { 0,0, 1,1, 2,2, 3,3, 3,4, 3,5 };
		cv::mixChannels( &new_resized_img, 1, mix_destination, 2, mix_pairing, 6 );
	}
	catch( cv::Exception cve ){
		blog( LOG_ERROR, "[lazysplits][SharedData] error making watch image for %s; %s!", GetName().c_str(), cve.msg.c_str() );
		return false;
	}
	return true;
}

bool LzsWatchImageBase::CheckBounds(){
	int area_x_neg_offset = std::max( 0, area_.x );
	int area_y_neg_offset = std::max( 0 , area_.y );
	cv::Rect source_rect( 0, 0, current_source_width_, current_source_height_ );

	//intersection of current watch area and source dimensions
	cv::Rect new_area = source_rect & area_;
	if( new_area.width <= 0 || new_area.height <=0 ){
		blog( LOG_DEBUG, "[lazysplits][SharedData] error checking watch bounds for %s; watch area does not intersect with source!", GetName().c_str() );
		return false;
	}
	else if( area_ != new_area ){
		int area_img_width_diff = area_.width - img_BGR_.cols;
		int area_img_height_diff = area_.height - img_BGR_.rows;

		area_ = new_area;
		cv::Rect img_crop( area_x_neg_offset, area_y_neg_offset, ( area_.width - area_img_width_diff ), ( area_.height - area_img_height_diff ) );
		img_BGR_ = img_BGR_(img_crop);
		img_mask_ = img_mask_(img_crop);
	}

	return true;
}

/* LzsWatchImageStatic */

LzsWatchImageStatic::LzsWatchImageStatic( const Proto::WatchInfo& watch_info, int watch_index, std::string watch_dir )
	:LzsWatchImageBase( watch_info, watch_index, watch_dir )
{}


bool LzsWatchImageStatic::FindWatch( const cv::Mat& BGR_frame, const SendableCalibrationProps& calib_props ){
	ValidateData( BGR_frame.cols, BGR_frame.rows, calib_props );
	if( IsGood() ){
		cv::Mat cropped_frame = BGR_frame(area_);
		return ImgProc::FindImage( cropped_frame, img_BGR_, img_mask_, 0.95F );
	}
	else{
		return false;
	}

}

bool LzsWatchImageStatic::RemakeData(){
	return ( MakeArea() && MakeImage()  && CheckBounds() );
}


} //namepsace SharedData
} //namespace Lazysplits