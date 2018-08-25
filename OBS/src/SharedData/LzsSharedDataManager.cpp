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

bool LzsSharedDataManager::TryConstructTargetList( const Proto::CsMessage& cs_msg, std::vector<std::shared_ptr<SharedData::LzsTarget>>& target_list ){
	target_list.clear();
	const std::string game_name = cs_msg.game_name();

	//if target game doesn't match current one, try to reparse it
	if( game_name != current_game_.GetCurrentGameName() && !SetGame(game_name) ){
		blog( LOG_WARNING, "[Lazysplits][shared_data] Failed to construct target list for %s; failed to parse/reparse game from \"%s\"!",
			game_name.c_str(),
			current_game_.GetCurrentGameName().c_str()
		);

		return false;
	}

	auto targets = cs_msg.targets();
	for( auto target_it = targets.begin(); target_it != targets.end(); ++target_it ){
		Proto::TargetInfo target_info;
		if( current_game_.GetTargetInfo( target_it->target_name(), target_info ) ){
			/*
			//get watch vars
			auto in_watch_vars = target_it->watch_variables();
			for( auto watch_var_it = in_watch_vars.begin(); watch_var_it != in_watch_vars.end(); ++watch_var_it ){
				if( watch_var_it->name().empty() || watch_var_it->value().empty() ){ continue; }

				Proto::TargetInfo::WatchVar new_watch_var;
				new_watch_var.set_index( watch_var_it->index() );
				new_watch_var.set_name( watch_var_it->name() );
				new_watch_var.set_val( watch_var_it->value() );

				target_info.


				auto new_watch_var = std::make_shared<LzsWatchVar>();
				new_watch_var->watch_index_ = watch_var_it->index();
				new_watch_var->watch_name_ = watch_var_it->name();
				new_watch_var->value_ = watch_var_it->value();
				out_watch_vars.push_back(new_watch_var);
			}
			*/

			auto new_target = std::make_shared<LzsTarget>( current_game_.GetCurrentGamePath(), target_info );
			if( new_target->ParseWatchList( target_it->watch_variables() ) ){
				target_list.push_back(new_target);
				blog( LOG_DEBUG, "[Lazysplits][shared_data] %s target added.", target_it->target_name().c_str() );
			}
		}
		else{
			blog( LOG_WARNING,"[Lazysplits][shared_data] Failed to parse target %s for %s!",
				target_it->target_name().c_str(),
				game_name.c_str()
			);
		}
	}

	return true;
}

} //namespace SharedData
} //namespace Lazysplits