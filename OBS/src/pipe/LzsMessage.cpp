#include "LzsMessage.h"

namespace Lazysplits{

CvMsg::CvMsg( CV_MESSAGE_TYPE type ){ type_ = type; }

CvPipeConnectionMsg::CvPipeConnectionMsg( bool pipe_connected )
	:CvMsg(CV_PIPE_CONNECTION_MSG)
{
	pipe_connected_ = pipe_connected;
}

CvPipeProtobufMsg::CvPipeProtobufMsg( std::string serialized_protobuf )
	:CvMsg(CV_PIPE_PROTOBUF_MSG)
{
	serialized_protobuf_ = serialized_protobuf;
}
	
CvSharedPathMsg::CvSharedPathMsg( std::string shared_data_path )
	:CvMsg(CV_SHARED_DATA_PATH_MSG)
{
	shared_data_path_ = shared_data_path;
}

} //namespace Lazysplits