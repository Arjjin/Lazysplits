#pragma once

#include "util\LzsCalibrationData.h"

#include <string>

namespace Lazysplits{

enum CV_MESSAGE_TYPE{ NONE, CV_PIPE_CONNECTION_MSG, CV_PIPE_PROTOBUF_MSG, CV_SHARED_DATA_PATH_MSG, CV_CALIBRATION_DATA_MSG };

class CvMsg{
	public :
		CvMsg( CV_MESSAGE_TYPE type );
		CV_MESSAGE_TYPE type_;
};

class CvPipeConnectionMsg : public CvMsg{
	public :
		CvPipeConnectionMsg( bool pipe_connected );
		bool pipe_connected_;
};

class CvPipeProtobufMsg : public CvMsg{
	public :
		CvPipeProtobufMsg( std::string serialized_protobuf );
		std::string serialized_protobuf_;
};

class CvSharedPathMsg : public CvMsg{
	public :
		CvSharedPathMsg( const std::string shared_data_path );
		std::string shared_data_path_;
};

class CvCalibrationDataMsg : public CvMsg{
	public :
		CvCalibrationDataMsg( const SendableCalibrationProps calib_props );
		SendableCalibrationProps calib_props_;
};

} //namespace Lazysplits