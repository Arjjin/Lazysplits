#include "SharedData\LzsSharedData.h"
#include "util\LzsProtoHelper.h"

#include <filesystem>

namespace filesys = std::experimental::filesystem;

namespace Lazysplits{
namespace SharedData{

LzsTarget::LzsTarget( const Proto::TargetInfo& target_info, const std::string& game_info_dir, std::map<std::string,std::string> watch_vars ){
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
			switch( watch_info.type() ){
				case Proto::WatchType::WT_COLOR :
					watches_.push_back( std::make_shared<LzsWatchColor>( watch_info, watches_it->index(), watch_path.string() ) );
				break;
				case Proto::WatchType::WT_IMAGE_STATIC :
					watches_.push_back( std::make_shared<LzsWatchImageStatic>( watch_info, watches_it->index(), watch_path.string() ) );
				break;
				case Proto::WatchType::WT_CHARACTER_SET :
					try{
						std::string watch_var = watch_vars_.at( watch_info.name() );
						watches_.push_back( std::make_shared<LzsWatchCharacterSet>( watch_info, watches_it->index(), watch_path.string(), watch_var ) );
					}
					catch( std::out_of_range e ){
						blog( LOG_WARNING, "[lazysplits][SharedData]  target '%s', watch var not found for '%s' (not set in splits?)",
							GetName().c_str(),
							watch_info.name().c_str() );
					}
				break;
			}
			if( latest_watch_index < watches_it->index() ){ 
				latest_watch_index = watches_it->index();
			}
		}
		else{ return false; }
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

} //namepsace SharedData
} //namespace Lazysplits