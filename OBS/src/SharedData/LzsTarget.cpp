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
	is_complete_ = false;
}

bool LzsTarget::ParseWatchList(){
	auto watches = target_info_.watches();

	for( auto watches_it = watches.begin(); watches_it != watches.end(); ++watches_it ){
		filesys::path watch_path = game_info_dir_;
		watch_path /= watches_it->relative_path();
		filesys::path watch_file_path = watch_path;
		watch_file_path /= "WatchInfo.json";

		//check that WatchInfo exists at path
		if( !filesys::exists(watch_file_path) ){
			blog( LOG_WARNING, "[Lazysplits][target]  failed to parse watch for target %s; file (%s) does not exist!",
				target_info_.name().c_str(),
				watch_file_path.c_str()
			);
			return false;
		}

		//parse from path
		Proto::WatchInfo watch_info;
		if( Proto::JsonFileToProto( watch_file_path.string(), &watch_info ) ){
			int watch_index = watches_it->index();
			std::string watch_var = FindWatchVar( watch_index, watch_info.name() );
			Proto::WatchAction watch_action = watches_it->action();
			int watch_action_val = watches_it->action_val();

			switch( watch_info.type() ){
				case Proto::WatchType::WT_COLOR :
					watches_.push_back(
						std::make_shared<LzsWatchColor>( watch_info, *watches_it, watch_path.string(), watch_var )
					);
				break;
				case Proto::WatchType::WT_IMAGE_STATIC :
					watches_.push_back(
						std::make_shared<LzsWatchImageStatic>( watch_info, *watches_it, watch_path.string(), watch_var )
					);
				break;
				case Proto::WatchType::WT_CHARACTER_SET :
					watches_.push_back(
						std::make_shared<LzsWatchCharacterSet>( watch_info, *watches_it, watch_path.string(), watch_var )
					);
				break;
			}
		}
		else{
			blog( LOG_WARNING, "[Lazysplits][target] failed to parse watch %s for target %s!", watches_it->name().c_str(), target_info_.name().c_str() );
			return false;
		}
	}

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
		int index = (*watch_it)->GetIndex();
		Proto::WatchPersistence persistence = (*watch_it)->GetPersistence();
		
		if( persistence == Proto::WatchPersistence::WP_NONE && index != current_watch_index_ ){
			continue;
		}
		else if( persistence == Proto::WatchPersistence::WP_SPECIFY_MAX ){
			int persistence_max = (*watch_it)->GetPersistenceMax();
			if( index > current_watch_index_ || persistence_max < current_watch_index_ ){
				continue;
			}
		}
		else if( persistence == Proto::WatchPersistence::WP_TOTAL && index > current_watch_index_ ){
			continue;
		}

		current_watches.push_back(*watch_it);
	}

	return current_watches;
}

void LzsTarget::WatchAction( Proto::WatchAction action, int action_val ){
	switch(action){
		case Proto::WatchAction::WA_INCREMENT_INDEX :
			current_watch_index_++;
			blog( LOG_DEBUG, "[Lazysplits][target] watch index incremented (%i) for target %s", current_watch_index_, target_info_.name().c_str() );
		break;
		case Proto::WatchAction::WA_DECREMENT_INDEX :
			if( current_watch_index_ > 0 ){
				current_watch_index_--;
				blog( LOG_DEBUG, "[Lazysplits][target] watch index decremented (%i) for target %s", current_watch_index_, target_info_.name().c_str() );
			}
			else{
				blog( LOG_WARNING, "[Lazysplits][target] target %s attempted to increment index of 0 or less!", target_info_.name().c_str() );
			}
		break;
		case Proto::WatchAction::WA_GOTO_INDEX :
			if( action_val >= 0 ){
				current_watch_index_ = action_val;
				blog( LOG_DEBUG, "[Lazysplits][target] watch index changed (%i) for target %s", current_watch_index_, target_info_.name().c_str() );
			}
			else{
				blog( LOG_WARNING, "[Lazysplits][target] target %s attempted to GOTO index of 0 or less!", target_info_.name().c_str() );
			}
		break;
		case Proto::WatchAction::WA_COMPLETE :
			is_complete_ = true;
		break;
	}
}

bool LzsTarget::IsComplete(){
	return is_complete_;
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