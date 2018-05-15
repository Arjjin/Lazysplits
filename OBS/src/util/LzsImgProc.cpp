#include "LzsImgProc.h"

#include <opencv2\imgproc.hpp>

namespace Lazysplits{
namespace ImgProc{


bool WatchExists( const cv::Mat& source, cv::Mat& watch_BGR, cv::Mat& watch_A, float threshold ){
	bool img_found = false;
	try{
		cv::Mat result;
		cv::matchTemplate( source, watch_BGR, result, CV_TM_CCORR_NORMED, watch_A );
		
		double min = 0.0F;
		double max = 0.0F;
		cv::minMaxLoc( result, &min, &max );
		//blog( LOG_ERROR, "[lazysplits][ImgProc] min : %f, max : %f", (float)min, (float)max );

		double found = max;
		
		if( found >= threshold ){ img_found = true; }
	}
	catch( cv::Exception cve ){
		blog( LOG_ERROR, "[lazysplits][ImgProc] error matching template; %s", cve.msg.c_str() );
	}
	return img_found;
}

} //namespace ImgProc
} //namespace Lazysplits