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

LzsWatchCharacterSet::LzsWatchCharacterSet(
	const Proto::WatchInfo& watch_info,
	const Proto::TargetInfo_WatchEntry& watch_entry,
	const std::string& watch_dir,
	const std::string& watch_var
)
	:LzsWatchImageBase( watch_info, watch_entry, watch_dir, watch_var )
{
	//make an internal map of our character entries
	MakeCharMap();
	//create an internal container of character entries that corrspond to our character_input string from it
	MakeCharInput(watch_var_);
	 
	/*
	for( auto it = character_input_.begin(); it != character_input_.end(); ++it ){
		blog( LOG_DEBUG, "CHAR ENTRY; index : %i, val : %s, padding : (%ix%i)", 
			it->character_index(),
			it->character_val().c_str(),
			it->x_padding(),
			it->y_padding()
		);
	}
	*/
}

bool LzsWatchCharacterSet::CvLogic( const cv::Mat& BGR_frame ){
	cv::Mat cropped_frame = BGR_frame(area_);

	return ImgProc::FindImage( cropped_frame, img_BGR_, img_mask_, watch_info_.base_threshold() );
}

bool LzsWatchCharacterSet::MakeArea(){
	if( character_input_.empty() ){ return false; }

	uint32_t char_width = watch_info_.area().size().x();
	uint32_t char_height = watch_info_.area().size().y();

	//int additional_padding = watch_info_.additional_area_padding();
	int area_x = watch_info_.area().loc().x();
	int area_y = watch_info_.area().loc().y();
	int area_width = char_width * character_input_.size();
	int area_height = char_height;

	int x_padding = 0;
	int y_padding = 0;
	//adjust for individual character padding
	for( auto input_char_it = character_input_.begin(); input_char_it != character_input_.end(); ++input_char_it ){
		x_padding += input_char_it->x_padding();
		y_padding += input_char_it->y_padding();
	}
	//don't count padding of first character (last in container when starting from right)
	if( watch_info_.character_z_order() == Proto::CharacterZOrder::CHARACTER_Z_LEFT_TO_RIGHT ){
		x_padding -= character_input_.begin()->x_padding();
	}
	else{
		x_padding -= ( character_input_.end() - 1 )->x_padding();
	}

	//adjust for left/right justification
	//TODO take y padding into account
	cv::Point2i additional_area_offset = GetAdditonalAreaOffset();
	switch( watch_info_.character_justify() ){
		case Proto::CharacterJustify::CHARACTER_JUSTIFY_LEFT :
			area_width += x_padding;

			area_x += additional_area_offset.x;
		break;
		case Proto::CharacterJustify::CHARACTER_JUSTIFY_RIGHT :
			area_width += x_padding;

			//invert values for right justified watch, and subtract base width of watch
			area_x -= additional_area_offset.x+area_width;
		break;
	}

	//calibration
	int watch_base_width = watch_info_.base_dimensions().x();
	int watch_base_height = watch_info_.base_dimensions().y();

	float width_multiplier = (float)current_source_width_/watch_base_width;
	float height_multiplier = (float)current_source_height_/watch_base_height;
	float x_multiplier = 1.0;
	float y_multiplier = 1.0;

	if( current_calib_props_.is_enabled ){
		float calib_x_multiplier = ( (float)current_calib_props_.img_width/watch_base_width ) * ( current_calib_props_.scale_x / 100.0F );
		float calib_y_multiplier = ( (float)current_calib_props_.img_height/watch_base_height ) * ( current_calib_props_.scale_y / 100.0F );

		width_multiplier *= calib_x_multiplier;
		height_multiplier *= calib_y_multiplier;
		x_multiplier *= calib_x_multiplier;
		y_multiplier *= calib_y_multiplier;
	}

	int new_x = ( area_x * width_multiplier ) + ( current_calib_props_.loc_x * x_multiplier );
	int new_y = ( area_y * height_multiplier ) + ( current_calib_props_.loc_y * y_multiplier );

	int additional_padding = watch_info_.additional_area_padding();
	area_.x = new_x - additional_padding;
	area_.y = new_y - additional_padding;
	area_.width = ( area_width * width_multiplier ) + ( additional_padding * 2 );
	area_.height = ( area_height * height_multiplier ) + ( additional_padding * 2 );

	if( area_.width <= 0 || area_.height <= 0 ){
		blog( LOG_DEBUG, "[lazysplits][SharedData] error making watch area for %s; width or height <= 0!", GetName().c_str() );
		return false;
	}
	return true;
}

bool LzsWatchCharacterSet::MakeImage(){
	if( character_input_.empty() ){ return false; }

	filesys::path img_path = watch_dir_;
	img_path /= watch_info_.img_relative_path();

	try{
		cv::Mat character_set = cv::imread( img_path.string(), cv::IMREAD_UNCHANGED );
		if( character_set.cols <= 0 || character_set.rows <= 0 ){
			blog( LOG_ERROR, "[lazysplits][SharedData] error reading character set image (%s) for watch %s; width or height is <= 0!", img_path.string().c_str(), GetName().c_str() );
			return false;
		}
		
		uint32_t char_width = watch_info_.area().size().x();
		uint32_t char_height = watch_info_.area().size().y();
		int image_width = char_width * character_input_.size();
		int image_height = char_height;

		int x_padding = 0;
		int y_padding = 0;
		//adjust for individual character padding
		for( auto input_char_it = character_input_.begin(); input_char_it != character_input_.end(); ++input_char_it ){
			x_padding += input_char_it->x_padding();
			y_padding += input_char_it->y_padding();
		}
		
		int prev_x_loc;
		if( watch_info_.character_z_order() == Proto::CHARACTER_Z_LEFT_TO_RIGHT ){
			//don't count padding of first character
			x_padding -= character_input_.begin()->x_padding();
			image_width += x_padding;

			prev_x_loc = 0;
		}
		else{
			//don't count padding of first character (last in container when starting from right)
			x_padding -= ( character_input_.end() - 1 )->x_padding();
			image_width += x_padding;

			prev_x_loc = image_width;
		}

		cv::Mat composite_image( image_height, image_width, CV_8UC4, cv::Scalar( 0.0, 0.0, 0.0, 0.0 ) );

		for( auto input_char_it = character_input_.begin(); input_char_it != character_input_.end(); ++input_char_it ){
			int index = std::distance( character_input_.begin(), input_char_it );
			int inv_index = ( character_input_.size() - 1 ) - index;
			
			
			if( watch_info_.character_z_order() == Proto::CHARACTER_Z_LEFT_TO_RIGHT ){
				const Proto::WatchInfo_CharacterEntry& char_entry = character_input_.at(index);
				//pull individual character image from character set
				cv::Mat char_image = character_set( cv::Rect( char_entry.character_index()*char_width, 0, char_width, char_height ) );

				int cur_x_loc = prev_x_loc + char_entry.x_padding();
				//range clamping
				cur_x_loc = std::max<int>( 0, cur_x_loc );
				cur_x_loc = std::min<int>( image_width-char_width, cur_x_loc );
				//copy to our composite image
				char_image.copyTo(
					composite_image( cv::Rect(
						//std::max<int>( 0, ( index * char_width ) + char_entry.x_padding() ),
						cur_x_loc,
						0,
						char_width,
						char_height
					) ),
					char_image
				);

				prev_x_loc = cur_x_loc + char_width;
			}
			else{
				const Proto::WatchInfo_CharacterEntry& char_entry = character_input_.at(inv_index);
				//pull individual character image from character set
				cv::Mat char_image = character_set( cv::Rect( char_entry.character_index()*char_width, 0, char_width, char_height ) );

				int cur_x_loc = prev_x_loc - char_width - char_entry.x_padding();
				//range clamping
				cur_x_loc = std::max<int>( 0, cur_x_loc );
				cur_x_loc = std::min<int>( image_width-char_width, cur_x_loc );
				//copy to our composite image
				char_image.copyTo(
					composite_image( cv::Rect(
						//std::min<int>( composite_image.cols-char_width, ( composite_image.cols - ( char_width * index ) ) + char_entry.x_padding() ),
						cur_x_loc,
						0,
						char_width,
						char_height
					) ),
					char_image
				);

				prev_x_loc = cur_x_loc;
			}
		}
		
		//calibration
		int watch_base_width = watch_info_.base_dimensions().x();
		int watch_base_height = watch_info_.base_dimensions().y();
		float width_multiplier = (float)current_source_width_/watch_base_width;
		float height_multiplier = (float)current_source_height_/watch_base_height;

		if( current_calib_props_.is_enabled ){
			float calib_x_multiplier = ( (float)watch_base_width/ current_calib_props_.img_width ) * ( current_calib_props_.scale_x / 100.0F );
			float calib_y_multiplier = ( (float)watch_base_height/ current_calib_props_.img_height ) * ( current_calib_props_.scale_y / 100.0F );

			width_multiplier *= calib_x_multiplier;
			height_multiplier *= calib_y_multiplier;
		}

		int new_width = composite_image.cols * width_multiplier;
		int new_height = composite_image.rows * height_multiplier;

		/* moving BGRA mat into BGR and mask mats */

		img_BGR_ = cv::Mat( composite_image.rows, composite_image.cols, CV_8UC3 );
		img_mask_ = cv::Mat( composite_image.rows, composite_image.cols, CV_8UC3 );
		cv::Mat mix_destination[] = { img_BGR_, img_mask_ };
		int mix_pairing[] = { 0,0, 1,1, 2,2, 3,3, 3,4, 3,5 };
		cv::mixChannels( &composite_image, 1, mix_destination, 2, mix_pairing, 6 );

		//resize BGR with linear interp and bitmask with NN
		cv::resize( img_BGR_, img_BGR_, cv::Size( new_width, new_height ) );
		cv::resize( img_mask_, img_mask_, cv::Size( new_width, new_height ), 0.0, 0.0, cv::INTER_NEAREST ); 
	}
	catch( cv::Exception cve ){
		blog( LOG_ERROR, "[lazysplits][SharedData] error making watch image for %s; %s!", GetName().c_str(), cve.msg.c_str() );
		return false;
	}
	return true;
}

bool LzsWatchCharacterSet::RemakeData(){
	return ( MakeArea() && MakeImage()  && CheckBounds() );
	//return ( MakeArea() );
}

void LzsWatchCharacterSet::MakeCharMap(){
	auto proto_char_entries = watch_info_.characters();
	for( auto char_entry_it = proto_char_entries.begin(); char_entry_it != proto_char_entries.end(); ++ char_entry_it ){
		character_map_.emplace( char_entry_it->character_val().at(0), *char_entry_it );
	}
}

void LzsWatchCharacterSet::MakeCharInput( const std::string& input_string ){
	//if we ever start using UTF8 this is gonna break pretty bad
	for( int i = 0; i < input_string.length(); i++ ){
		auto char_entry_it = character_map_.find( input_string.at(i) );
		if( char_entry_it != character_map_.end() ){
			character_input_.push_back( char_entry_it->second );
		}
	}
}

const cv::Point2i LzsWatchCharacterSet::GetAdditonalAreaOffset(){
	cv::Point2i offset( 0, 0 );

	auto additional_offset_entries = watch_info_.additional_offset();
	for( auto offset_it = additional_offset_entries.begin(); offset_it != additional_offset_entries.end(); ++offset_it ){
		if( character_input_.size() >= offset_it->character_length_threshold() ){
			auto dims = offset_it->offset();
			offset.x = dims.x();
			offset.y = dims.y();
		}
	}

	return offset;
}

} //namepsace SharedData
} //namespace Lazysplits