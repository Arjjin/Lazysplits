#include "LzsImgProc.h"

#include <opencv2\imgproc.hpp>

namespace Lazysplits{
namespace ImgProc{

cv::Mat FrameToBGRMat( obs_source_frame* frame ){
	cv::Mat BGR_out;

	try{
		switch(frame->format){
			case VIDEO_FORMAT_I420 :
				BGR_out = cv::Mat( frame->height*1.5F, frame->width, CV_8UC1, frame->data[0] );
				cv::resize( BGR_out, BGR_out, cv::Size(), 0.25F, 0.25F, cv::INTER_AREA );
				cv::cvtColor( BGR_out, BGR_out, CV_YUV2BGR_I420 );
			break;
			case VIDEO_FORMAT_NV12 :
				BGR_out = cv::Mat( frame->height*1.5F, frame->width, CV_8UC1, frame->data[0] );
				cv::resize( BGR_out, BGR_out, cv::Size(), 0.25F, 0.25F, cv::INTER_AREA );
				cv::cvtColor( BGR_out, BGR_out, CV_YUV2BGR_NV12 );
			break;
			case VIDEO_FORMAT_YVYU :
				BGR_out = cv::Mat( frame->height, frame->width, CV_8UC2, frame->data[0] );
				cv::resize( BGR_out, BGR_out, cv::Size(), 0.25F, 0.25F, cv::INTER_AREA );
				cv::cvtColor( BGR_out, BGR_out, CV_YUV2BGR_YVYU );
			break;
			case VIDEO_FORMAT_YUY2 :
				BGR_out = cv::Mat( frame->height, frame->width, CV_8UC2, frame->data[0] );
				cv::cvtColor( BGR_out, BGR_out, CV_YUV2BGR_YUY2 );
				cv::resize( BGR_out, BGR_out, cv::Size(), 320.0F/frame->width, 240.0F/frame->height, cv::INTER_LINEAR );
			break;
			case VIDEO_FORMAT_UYVY :
				BGR_out = cv::Mat( frame->height, frame->width, CV_8UC2, frame->data[0] );
				cv::resize( BGR_out, BGR_out, cv::Size(), 0.25F, 0.25F, cv::INTER_AREA );
				cv::cvtColor( BGR_out, BGR_out, CV_YUV2BGR_UYVY );
			break;
			case VIDEO_FORMAT_RGBA :
				BGR_out = cv::Scalar( 0, 255, 0 );
			break;
			case VIDEO_FORMAT_BGRA :
				BGR_out = cv::Scalar( 0, 255, 0 );
			break;
			case VIDEO_FORMAT_BGRX :
				BGR_out = cv::Scalar( 0, 255, 0 );
			break;
			case VIDEO_FORMAT_Y800 :
				BGR_out = cv::Scalar( 0, 255, 0 );
			break;
			case VIDEO_FORMAT_NONE :
				BGR_out = cv::Scalar( 0, 0, 255 );
			break;
		}
	}
	catch( cv::Exception cve ){
		blog( LOG_WARNING, "[Lazysplits] error converting raw frame : %s", cve.msg );
	}
	
	return BGR_out;
}

} //namespace ImgProc
} //namespace Lazysplits