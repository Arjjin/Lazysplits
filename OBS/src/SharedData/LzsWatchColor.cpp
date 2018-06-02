//#define NOMINMAX

#include "SharedData\LzsSharedData.h"
#include "util\LzsImgProc.h"

namespace Lazysplits{
namespace SharedData{

LzsWatchColor::LzsWatchColor(
	const Proto::WatchInfo& watch_info,
	const Proto::TargetInfo_WatchEntry& watch_entry,
	const std::string& watch_dir,
	const std::string& watch_var
)
	:LzsWatchBase( watch_info, watch_entry, watch_dir, watch_var )
{
	auto color = watch_info.color();
	RGB_scalar_ = cv::Scalar( color.r(), color.g(), color.b(), 255.0 );
}

bool LzsWatchColor::CvLogic( const cv::Mat& BGR_frame ){
	cv::Mat cropped_frame = BGR_frame(area_);
	return ImgProc::FindColor( cropped_frame, RGB_scalar_, watch_info_.base_threshold() );
}

bool LzsWatchColor::RemakeData(){
	return ( MakeArea() && CheckBounds() );
}

} //namepsace SharedData
} //namespace Lazysplits