#include "SharedData\LzsSharedData.h"

namespace Lazysplits{
namespace SharedData{

LzsWatch::LzsWatch( const Proto::WatchInfo& watch_info ){ watch_info_ = watch_info; }

const std::string& LzsWatch::GetName(){ return watch_info_.name(); }

Proto::WatchType LzsWatch::GetType(){ return watch_info_.type(); }

} //namepsace SharedData
} //namespace Lazysplits