#include "LzsImgProc.h"

#include <opencv2\imgproc.hpp>

#include <algorithm>

namespace Lazysplits{
namespace ImgProc{

bool FindColor( const cv::Mat& source_BGR, const cv::Scalar& watch_RGB_scalar, float threshold ){
	//clamp threshold to 0-1 range
	std::max<float>( 0.0, std::min<float>( 1.0, threshold ) );

	cv::Scalar source_BGR_mean = cv::mean(source_BGR);
	double R_dev =  abs( ( source_BGR_mean[2] - watch_RGB_scalar[0] ) ) / 255.0;
	double G_dev =  abs( ( source_BGR_mean[1] - watch_RGB_scalar[1] ) ) / 255.0;
	double B_dev =  abs( ( source_BGR_mean[0] - watch_RGB_scalar[2] ) ) / 255.0;
	double total_dev = ( R_dev + G_dev + B_dev ) / 3.0;

	return ( total_dev <= ( 1.0 - threshold ) );
}

bool FindImage( const cv::Mat& source_BGR, const cv::Mat& watch_BGR, const cv::Mat& watch_A, float threshold ){
	//clamp threshold to 0-1 range
	std::max<float>( 0.0, std::min<float>( 1.0, threshold ) );

	try{
		cv::Mat result;
		cv::matchTemplate( source_BGR, watch_BGR, result, cv::TM_CCORR_NORMED, watch_A );
		
		double min = 0.0F;
		double max = 0.0F;
		cv::minMaxLoc( result, &min, &max );
		//blog( LOG_ERROR, "[lazysplits][ImgProc] min : %f, max : %f", (float)min, (float)max );
		
		if( max >= threshold ){ return true; }
	}
	catch( cv::Exception cve ){
		blog( LOG_ERROR, "[lazysplits][ImgProc] error matching template; %s", cve.msg.c_str() );
	}
	return false;
}

} //namespace ImgProc
} //namespace Lazysplits