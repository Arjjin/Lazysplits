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

LzsWatchImageBase::LzsWatchImageBase(
	const Proto::WatchInfo& watch_info,
	const Proto::TargetInfo_WatchEntry& watch_entry,
	const std::string& watch_dir,
	const google::protobuf::RepeatedPtrField<Proto::CsMessage_WatchVariable>& watch_vars
)
	:LzsWatchBase( watch_info, watch_entry, watch_dir, watch_vars )
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
		int new_width = new_img.cols * x_multiplier;
		int new_height = new_img.rows * y_multiplier;

		/* moving BGRA mat into BGR and mask mats */

		img_BGR_ = cv::Mat( new_img.rows, new_img.cols, CV_8UC3 );
		img_mask_ = cv::Mat( new_img.rows, new_img.cols, CV_8UC3 );
		cv::Mat mix_destination[] = { img_BGR_, img_mask_ };
		int mix_pairing[] = { 0,0, 1,1, 2,2, 3,3, 3,4, 3,5 };
		cv::mixChannels( &new_img, 1, mix_destination, 2, mix_pairing, 6 );

		//resize BGR with linear interp (unless manually overridden) and bitmask with NN
		cv::resize(
			img_BGR_,
			img_BGR_,
			cv::Size( new_width, new_height ),
			0.0,
			0.0,
			current_calib_props_.use_nn_interp ? cv::INTER_NEAREST : cv::INTER_LINEAR
		); 
		cv::resize(
			img_mask_,
			img_mask_,
			cv::Size( new_width, new_height ),
			0.0,
			0.0,
			cv::INTER_NEAREST
		); 
	}
	catch( cv::Exception cve ){
		blog( LOG_ERROR, "[lazysplits][SharedData] error making watch image for %s; %s!", GetName().c_str(), cve.msg.c_str() );
		return false;
	}
	return true;
}

bool LzsWatchImageBase::CheckBounds(){
	//int area_x_neg_offset = std::min( 0, area_.x );
	//int area_y_neg_offset = std::min( 0 , area_.y );
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
		int additonal_area_padding = watch_info_.additional_area_padding();

		area_ = new_area;
		cv::Rect img_crop(
			//area_x_neg_offset + additonal_area_padding,
			//area_y_neg_offset + additonal_area_padding,
			additonal_area_padding,
			additonal_area_padding,
			( area_.width - area_img_width_diff ) - ( additonal_area_padding * 2 ),
			( area_.height - area_img_height_diff ) - ( additonal_area_padding * 2 )
		);
		img_BGR_ = img_BGR_(img_crop);
		img_mask_ = img_mask_(img_crop);
	}

	return true;
}

/* LzsWatchImageStatic */

LzsWatchImageStatic::LzsWatchImageStatic(
	const Proto::WatchInfo& watch_info,
	const Proto::TargetInfo_WatchEntry& watch_entry,
	const std::string& watch_dir,
	const google::protobuf::RepeatedPtrField<Proto::CsMessage_WatchVariable>& watch_vars
)
	:LzsWatchImageBase( watch_info, watch_entry, watch_dir, watch_vars )
{}


bool LzsWatchImageStatic::CvLogic( const cv::Mat& BGR_frame ){
	cv::Mat cropped_frame = BGR_frame(area_);
	
	/*
	if( _DEBUG ){
		if( watch_info_.name() != "game mode screen" ){
			std::vector<int> compression_params;
			compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
			compression_params.push_back(1);

			cv::imwrite("source_frame.png ",BGR_frame,compression_params);
			cv::imwrite("cropped_source_frame.png",cropped_frame,compression_params);
			cv::imwrite("search_frame.png",img_BGR_,compression_params);
			cv::imwrite("search_frame_mask.png",img_mask_,compression_params);
		}
	}
	*/

	return ImgProc::FindImage( cropped_frame, img_BGR_, img_mask_, watch_info_.base_threshold() );
}

bool LzsWatchImageStatic::RemakeData(){
	return ( MakeArea() && MakeImage()  && CheckBounds() );
}


} //namepsace SharedData
} //namespace Lazysplits