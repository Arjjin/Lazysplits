#pragma once

#include <obs.h>
#include <opencv2\core.hpp>

namespace Lazysplits{
namespace ImgProc{

bool FindColor( const cv::Mat& source_BGR, const cv::Scalar& watch_RGB_scalar, float threshold );
bool FindImage( const cv::Mat& source_BGR, const cv::Mat& watch_BGR, const cv::Mat& watch_A, float threshold );

} //namespace ImgProc
} //namespace Lazysplits