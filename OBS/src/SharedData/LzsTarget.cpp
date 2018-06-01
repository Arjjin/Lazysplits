#include "SharedData\LzsSharedData.h"
#include "util\LzsProtoHelper.h"

#include <filesystem>

namespace filesys = std::experimental::filesystem;

namespace Lazysplits{
namespace SharedData{

LzsTarget::LzsTarget( const std::string& game_info_dir, const Proto::TargetInfo& target_info, std::vector<std::shared_ptr<LzsWatchVar>> watch_vars ){
	target_info_ = target_info;
	game_info_dir_ = game_info_dir;
	watch_vars_ = watch_vars;

	current_watch_index_ = 0;
	//should be properly set when watch list is parsed
	final_watch_index_ = 0;
}

bool LzsTarget::ParseWatchList(){
	auto watches = target_info_.watches();

	uint32_t latest_watch_index = 0;
	for( auto watches_it = watches.begin(); watches_it != watches.end(); ++watches_it ){
		filesys::path watch_path = game_info_dir_;
		watch_path /= watches_it->relative_path();
		filesys::path watch_file_path = watch_path;
		watch_file_path /= "WatchInfo.json";

		Proto::WatchInfo watch_info;
		if( Proto::JsonFileToProto( watch_file_path.string(), &watch_info ) ){
			int watch_index = watches_it->index();
			std::string watch_var = FindWatchVar( watch_index, watch_info.name() );

			switch( watch_info.type() ){
				case Proto::WatchType::WT_COLOR :
					watches_.push_back( std::make_shared<LzsWatchColor>( watch_info, watch_index, watch_path.string(), watch_var ) );
				break;
				case Proto::WatchType::WT_IMAGE_STATIC :
					watches_.push_back( std::make_shared<LzsWatchImageStatic>( watch_info, watch_index, watch_path.string(), watch_var ) );
				break;
				case Proto::WatchType::WT_CHARACTER_SET :
					watches_.push_back( std::make_shared<LzsWatchCharacterSet>( watch_info, watch_index, watch_path.string(), watch_var ) );
				break;
			}
			if( latest_watch_index < watches_it->index() ){ 
				latest_watch_index = watches_it->index();
			}
		}
		else{
			blog( LOG_WARNING, "[Lazysplits][target] failed to parse watch %s for target %s!", watches_it->name().c_str(), target_info_.name().c_str() );
			return false;
		}
	}
	final_watch_index_ = latest_watch_index;

	return true;
}

const std::string& LzsTarget::GetName(){
	return target_info_.name();
}

Proto::TargetType LzsTarget::GetType(){
	return target_info_.type();
}

uint64_t LzsTarget::GetSplitOffset(){
	return target_info_.split_offset_ms();
}

const std::vector<std::shared_ptr<LzsWatchBase>> LzsTarget::GetCurrentWatches(){
	std::vector<std::shared_ptr<LzsWatchBase>> current_watches;
	for( auto watch_it = watches_.begin(); watch_it != watches_.end(); ++watch_it ){
		if( (*watch_it)->GetIndex() == current_watch_index_ ){ current_watches.push_back(*watch_it); }
	}

	return current_watches;
}

void LzsTarget::NextWatch(){
	current_watch_index_++;
}

bool LzsTarget::TargetFound(){
	return current_watch_index_ > final_watch_index_;
}

const std::string LzsTarget::FindWatchVar( int watch_index, const std::string& watch_name ){
	std::string found_var = "";
	for( auto watch_var_it = watch_vars_.begin(); watch_var_it != watch_vars_.end(); ++watch_var_it ){
		if( (*watch_var_it)->watch_index_ == watch_index && (*watch_var_it)->watch_name_ == watch_name ){
			found_var = (*watch_var_it)->value_;
		}
	}

	return found_var;
}

} //namepsace SharedData
} //namespace Lazysplits