#pragma once

#include <obs.h>
#include <opencv2\core.hpp>

namespace Lazysplits{
namespace ImgProc{

cv::Mat FrameToBGRMat( obs_source_frame* frame );

} //namespace ImgProc
} //namespace Lazysplits