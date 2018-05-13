#pragma once

#include <opencv2\core.hpp>

namespace Lazysplits{
	const cv::Scalar LZS_SCALAR_UNITIALIZED( 0.0F, 255.0F, 0.0F );
	const cv::Mat LZS_MAT_UNITIALIZED( 15, 15, CV_8UC3, LZS_SCALAR_UNITIALIZED );
	const cv::Scalar LZS_SCALAR_ERROR( 255.0F, 0.0F, 255.0F );
	const cv::Mat LZS_MAT_ERROR( 15, 15, CV_8UC3, LZS_SCALAR_ERROR );
}