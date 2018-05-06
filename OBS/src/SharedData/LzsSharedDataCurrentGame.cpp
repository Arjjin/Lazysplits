#include "LzsSharedData.h"
#include "util\LzsProtoHelper.h"

#include <obs.h>

#include <fstream>
#include <sstream>
#include <filesystem>

namespace filesys = std::experimental::filesystem;

namespace Lazysplits{
namespace SharedData{

LzsCurrentGame::LzsCurrentGame(){
	is_set_ = false;
	full_path_ = "";
}

bool LzsCurrentGame::IsSet(){
	return is_set_;
}

void LzsCurrentGame::Unset(){
	is_set_ = false;
	game_info_.Clear();
}

bool LzsCurrentGame::ParseFromDir( const std::string& path ){
	full_path_ = path;

	filesys::path file_path = path;
	file_path /= "GameInfo.json";
	
	if( filesys::exists(file_path) && Proto::JsonFileToProto( file_path.string(), &game_info_ ) ){
		is_set_ = true;
		blog( LOG_DEBUG, "[Lazysplits][shared_data] GameInfo sucessfully parsed at %s", file_path.string().c_str() );
	}
	else{
		Unset();
		blog( LOG_WARNING, "[Lazysplits][shared_data] Failed to parse GameInfo at %s", file_path.string().c_str() );
	}

	return is_set_;
}

const std::string& LzsCurrentGame::GetCurrentGameName(){
	return game_info_.name();
}

const std::string& LzsCurrentGame::GetCurrentGamePath(){
	return full_path_;
}

bool LzsCurrentGame::GetTargetInfo( const std::string& target_name, Proto::TargetInfo& source_proto ){
	auto target_list = game_info_.targets();

	for( auto target_list_it = target_list.begin(); target_list_it != target_list.end(); ++target_list_it ){
		if( target_list_it->name() == target_name ){
			filesys::path target_path = full_path_;
			target_path /= target_list_it->relative_path();
			target_path /= "TargetInfo.json";
		
			return Proto::JsonFileToProto( target_path.string(), &source_proto );
		}
	}

	return false;
}

} //namespace SharedData
} //namespace Lazysplits