#include "SharedData\LzsSharedData.h"
#include "util\LzsProtoHelper.h"

#include <filesystem>

namespace filesys = std::experimental::filesystem;

namespace Lazysplits{
namespace SharedData{

LzsTarget::LzsTarget( const Proto::TargetInfo& target_info, const std::string& game_info_dir ){
	target_info_ = target_info;
	game_info_dir_ = game_info_dir;
}

bool LzsTarget::ParseWatchList(){
	auto watches = target_info_.watches();

	for( auto watches_it = watches.begin(); watches_it != watches.end(); ++watches_it ){
		filesys::path watch_path = game_info_dir_;
		watch_path /= watches_it->relative_path();
		watch_path /= "WatchInfo.json";

		Proto::WatchInfo watch_info;
		if( Proto::JsonFileToProto( watch_path.string(), &watch_info ) ){
			watch_list_.push_back( LzsWatch(watch_info) );
		}
		else{ return false; }
	}

	return true;
}

const std::string& LzsTarget::GetName(){
	return target_info_.name();
}

Proto::TargetType LzsTarget::GetType(){
	return target_info_.type();
}

} //namepsace SharedData
} //namespace Lazysplits