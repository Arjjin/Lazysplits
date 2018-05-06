#include "LzsProtoHelper.h"

#include <google\protobuf\util\json_util.h>

#include <fstream>
#include <sstream>
#include <filesystem>
#include <obs.h>

namespace filesys = std::experimental::filesystem;

namespace Lazysplits{
namespace Proto{

bool JsonFileToProto( const std::string& file_path, google::protobuf::Message* protobuf ){
	std::ifstream file_in( file_path, std::ifstream::binary );
	std::ostringstream oss;
	oss << file_in.rdbuf();

	google::protobuf::util::Status status = google::protobuf::util::JsonStringToMessage( oss.str(), protobuf );
	if( status.ok() ){ return true; }
	else{
		blog( LOG_WARNING, "[Lazysplits]Failed to deserialize %s, error : %s", file_path.c_str(), status.error_message().ToString().c_str() );
		return false;
	}
}

/*
bool SerializeRequestMessage( std::string& out_str, int32_t& message_id_ref ){
	CppMessage message;
	message.set_id(message_id_ref);
	message.set_type(Proto::CppMessage_MessageType::CppMessage_MessageType_REQUEST_CV_TARGET);

	message_id_ref++;
	return message.SerializeToString(&out_str);
}
*/

} //namespace Proto
} //namespace Lazysplits