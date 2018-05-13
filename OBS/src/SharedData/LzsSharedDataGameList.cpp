#include "LzsSharedData.h"
#include "util\LzsProtoHelper.h"

#include <obs.h>

#include <fstream>
#include <sstream>
#include <filesystem>

namespace filesys = std::experimental::filesystem;

namespace Lazysplits{
namespace SharedData{

void LzsGameList::ParseFromDir( const std::string& path ){
	filesys::path file_path = path;
	file_path /= "GameList.json";
	
	if( filesys::exists(file_path) && Proto::JsonFileToProto( file_path.string(), &game_list_ ) ){
		blog( LOG_DEBUG, "[Lazysplits][shared_data] GameList sucessfully parsed at %s", file_path.string().c_str() );
	}
	else{
		game_list_ = Proto::GameList();
		blog( LOG_WARNING, "[Lazysplits][shared_data] Failed to parse GamesList at %s", file_path.string().c_str() );
	}
}

bool LzsGameList::GameExists( const std::string& game_name ){
	for( int i = 0; i < game_list_.games_size(); i++ ){
		if( game_list_.games().Get(i).name() == game_name ){ return true; }
	}
	return false;
}

const std::string LzsGameList::GetGameDir( const std::string& game_name ){
	for( int i = 0; i < game_list_.games_size(); i++ ){
		const Proto::GameList::GameEntry& game_entry = game_list_.games().Get(i);
		if( game_entry.name() == game_name ){ return game_entry.relative_path(); }
	}
	return "game_not_found";
}

const Proto::GameList& LzsGameList::GetGameList(){
	return game_list_;
}

} //namespace SharedData
} //namespa