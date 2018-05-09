#pragma once

#include "LzsCvDataProtoCpp.pb.h"

#include <string>
#include <vector>
#include <memory>

namespace Lazysplits{
namespace SharedData{

class LzsWatch{
	public :
		LzsWatch( const Proto::WatchInfo& watch_info );

		const std::string& GetName();
		Proto::WatchType GetType();
	private :
		Proto::WatchInfo watch_info_;
};

class LzsTarget{
	public :
		LzsTarget( const Proto::TargetInfo& target_info, const std::string& game_info_dir  );
		bool ParseWatchList();

		const std::string& GetName();
		Proto::TargetType GetType();
	private :
		Proto::TargetInfo target_info_;
		std::string game_info_dir_;
		std::vector<LzsWatch> watch_list_;
};

class LzsCurrentGame{
	public :
		LzsCurrentGame();

		bool IsSet();
		void Unset();
		bool ParseFromDir( const std::string& path );
		const std::string& GetCurrentGameName();
		const std::string& GetCurrentGamePath();
		
		bool GetTargetInfo( const std::string& target_name, Proto::TargetInfo& source_proto );
		//bool GetWatchInfo( const std::string& target_name, Proto::WatchInfo& watch_proto );
	private:
		Proto::GameInfo game_info_;
		std::string full_path_;
		bool is_set_;
};

class LzsGameList{
	public :
		const std::string& GetGameDir( const std::string& game_name );

		void ParseFromDir( const std::string& path );
		bool GameExists( const std::string& game_name );
		const Proto::GameList& GetGameList();
	private :
		Proto::GameList game_list_;
};

class LzsSharedDataManager{
	public :
		std::string GetRootDir();
		bool SetRootDir( const std::string& path );
		bool IsMatchingRootDir( const std::string& path );

		bool SetGame( const std::string& game_name );

		bool TryConstructTarget( const std::string& game_name, const std::string& target_name, std::shared_ptr<LzsTarget>& source_target );
	private :
		std::string root_dir_;
		LzsGameList game_list_;
		LzsCurrentGame current_game_;

};

} //namespace SharedData
} //namespace Lazysplits