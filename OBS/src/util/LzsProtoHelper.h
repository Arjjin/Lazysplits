#pragma once

#include "LzsPipeProtoCpp.pb.h"

#include <string>

namespace Lazysplits{
namespace Proto{

bool JsonFileToProto( const std::string& file_path, google::protobuf::Message* protobuf );

//bool SerializeRequestMessage( std::string& out_str, int32_t& message_id_ref );

} //namespace Proto
} //namespace Lazysplits