#pragma once

#include <obs.h>
#include <opencv2\core.hpp>

namespace Lazysplits{
namespace ImgProc{

bool WatchExists( const cv::Mat& source, cv::Mat& watch_BGR, cv::Mat& watch_A, float threshold );

} //namespace ImgProc
} //namespace Lazysplits