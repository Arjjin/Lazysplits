#include "LzsSharedData.h"

#include <filesystem>
#include <obs.h>

namespace filesys = std::experimental::filesystem;

namespace Lazysplits{
namespace SharedData{

const std::string& LzsSharedDataManager::GetRootDir(){
	return root_dir_;
}

bool LzsSharedDataManager::SetRootDir( const std::string& path ){
	if( root_dir_ != path ){
		root_dir_ = path;
		game_list_.ParseFromDir(root_dir_);
		//reparse current game if one is set
		if( current_game_.IsSet() ){
			SetGame(current_game_.GetCurrentGameName() );
		}

		return true;
	}
	else{
		return false;
	}
}

bool LzsSharedDataManager::IsMatchingRootDir( const std::string& path ){
	filesys::path dir_path = path;
	dir_path.make_preferred();

	return dir_path == root_dir_;
}


const std::string& LzsSharedDataManager::GetGameName(){
	return current_game_.GetCurrentGameName();
}

bool LzsSharedDataManager::SetGame( const std::string& game_name ){
	filesys::path game_path = root_dir_;
	game_path /= game_list_.GetGameDir(game_name);

	return current_game_.ParseFromDir( game_path.string() );
}

bool LzsSharedDataManager::TryConstructTarget( const Proto::CsMessage& cs_msg, std::shared_ptr<LzsTarget>& source_target  ){
	const std::string game_name = cs_msg.game_name();
	const std::string target_name = cs_msg.target_name();

	//if target game doesn't match current one, try to reparse it
	if( game_name != current_game_.GetCurrentGameName() && !SetGame(game_name) ){
		blog( LOG_DEBUG, "[Lazysplits][shared_data] Failed to get target %s for %s; failed to parse/reparse game from \"%s\"!",
			target_name.c_str(),
			game_name.c_str(),
			current_game_.GetCurrentGameName().c_str()
		);

		return false;
	}
	
	Proto::TargetInfo target_info;
	if( current_game_.GetTargetInfo( target_name, target_info) ){
		//make watch variable list
		std::map<std::string,std::string> watch_vars;
		for( auto watch_var_it = cs_msg.watch_variables().begin(); watch_var_it != cs_msg.watch_variables().end(); ++watch_var_it ){
			if( watch_var_it->name().empty() || watch_var_it->value().empty() ){
				blog( LOG_WARNING, "[Lazysplits][shared_data] watch var for %s has empty name or value!", target_name.c_str() );
			}
			else{
				watch_vars.emplace( watch_var_it->name(), watch_var_it->value() );
			}
		}

		source_target = std::make_shared<LzsTarget>( target_info, current_game_.GetCurrentGamePath(), watch_vars );
		if( !source_target->ParseWatchList() ){
			return false;
		}
	}
	else{
		blog( LOG_DEBUG, "[Lazysplits][shared_data] Failed to get target %s for %s; failed to find TargetInfo.json!", target_name.c_str(), game_name.c_str() );
		return false;
	}

	return true;
}

} //namespace SharedData
} //namespace Lazysplits