#include "LzsProtoHelper.h"

namespace Lazysplits{
namespace Proto{

bool SerializeConnectionMessage( std::string& out_str, int32_t& message_id_ref, bool connected ){
	CppMessage message;
	message.set_message_id(message_id_ref);
	message.set_message_type(Proto::CppMessage_MessageType::CppMessage_MessageType_PIPE_INFO);
	message.set_pipe_connected(connected);
	
	message_id_ref++;
	return message.SerializeToString(&out_str);
}

bool SerializeRequestMessage( std::string& out_str, int32_t& message_id_ref ){
	CppMessage message;
	message.set_message_id(message_id_ref);
	message.set_message_type(Proto::CppMessage_MessageType::CppMessage_MessageType_REQUEST_CV_TARGET);

	message_id_ref++;
	return message.SerializeToString(&out_str);
}

} //namespace Proto
} //namespace Lazysplits