#pragma once

#include "LzsProtobufCpp.pb.h"
#include <string>

namespace Lazysplits{
namespace Proto{

bool SerializeConnectionMessage( std::string& out_str, int32_t& message_id_ref, bool connected );
bool SerializeRequestMessage( std::string& out_str, int32_t& message_id_ref );

} //namespace Proto
} //namespace Lazysplits