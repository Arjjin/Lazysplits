#pragma once

#include "util\LzsCalibrationData.h"
#include "LzsCvDataProtoCpp.pb.h"
#include "LzsPipeProtoCpp.pb.h"

#include <opencv2\core.hpp>

#include <string>
#include <vector>
#include <memory>

namespace Lazysplits{
namespace SharedData{

enum LzsWatchStatus{ LZS_WATCH_ERROR, LZS_WATCH_UNITIALIZED, LZS_WATCH_GOOD };

class LzsWatchBase{
	public :
		LzsWatchBase( const Proto::WatchInfo& watch_info, int watch_index, std::string watch_dir );

		bool IsGood();
		const std::string& GetName();
		Proto::WatchType GetType();
		int GetIndex();

		bool FindWatch( const cv::Mat& BGR_frame, const SendableCalibrationProps& calib_props );
		virtual bool CvLogic( const cv::Mat& BGR_frame ) = 0;
	protected :
		void SetError();
		void SetUnitialized();
		void SetGood();

		void ValidateData( int source_width, int source_height, const SendableCalibrationProps& calib_props );
		bool ShouldRemakeData( int source_width, int source_height, const SendableCalibrationProps& calib_props );
		virtual bool RemakeData() = 0;
		
		virtual bool MakeArea();
		virtual bool CheckBounds();
		bool IsDiffCalibProps( const SendableCalibrationProps& calib_props );

		Proto::WatchInfo watch_info_;
		LzsWatchStatus status_;
		int index_;
		std::string watch_dir_;

		cv::Rect area_;

		SendableCalibrationProps current_calib_props_;
		int current_source_width_;
		int current_source_height_;
};

class LzsWatchColor : public LzsWatchBase{
	public :
		LzsWatchColor( const Proto::WatchInfo& watch_info, int watch_index, std::string watch_dir );
		bool CvLogic( const cv::Mat& BGR_frame )override;
	private :
		//bool MakeArea()override;
		//bool CheckBounds()override;
		bool RemakeData()override;

		cv::Scalar RGB_scalar_;
};

class LzsWatchImageBase : public LzsWatchBase{
	public :
		LzsWatchImageBase( const Proto::WatchInfo& watch_info, int watch_index, std::string watch_dir );
	protected :
		virtual bool MakeImage();
		virtual bool CheckBounds()override;

		cv::Mat img_BGR_;
		cv::Mat img_mask_;
};

class LzsWatchImageStatic : public LzsWatchImageBase{
	public :
		LzsWatchImageStatic( const Proto::WatchInfo& watch_info, int watch_index, std::string watch_dir );
		bool CvLogic( const cv::Mat& BGR_frame )override;
	private :
		bool RemakeData()override;
};

class LzsWatchCharacterSet : public LzsWatchImageBase{
	public :
		LzsWatchCharacterSet( const Proto::WatchInfo& watch_info, int watch_index, std::string watch_dir, const std::string character_input );
		bool CvLogic( const cv::Mat& BGR_frame )override;
	private :
		bool MakeArea()override;
		bool MakeImage()override;
		bool RemakeData()override;

		void MakeCharMap();
		void MakeCharInput( const std::string& input_string );
		std::pair<int,int> GetAdditonalAreaOffset();

		//const std::string character_input_;
		std::map<char,Proto::WatchInfo_CharacterEntry> character_map_;
		std::vector<Proto::WatchInfo_CharacterEntry> character_input_;

};

class LzsTarget{
	public :
		LzsTarget( const Proto::TargetInfo& target_info, const std::string& game_info_dir, std::map<std::string,std::string> watch_vars );
		bool ParseWatchList();

		const std::string& GetName();
		Proto::TargetType GetType();
		uint64_t GetSplitOffset();
		const std::vector<std::shared_ptr<LzsWatchBase>> GetCurrentWatches();
		void NextWatch();
		bool TargetFound();
	private :
		Proto::TargetInfo target_info_;
		std::string game_info_dir_;
		std::vector<std::shared_ptr<LzsWatchBase>> watches_;
		std::map<std::string,std::string> watch_vars_;
		uint32_t current_watch_index_;
		uint32_t final_watch_index_;
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
	private:
		Proto::GameInfo game_info_;
		std::string full_path_;
		bool is_set_;
};

class LzsGameList{
	public :
		const std::string GetGameDir( const std::string& game_name );

		void ParseFromDir( const std::string& path );
		bool GameExists( const std::string& game_name );
		const Proto::GameList& GetGameList();
	private :
		Proto::GameList game_list_;
};

class LzsSharedDataManager{
	public :
		const std::string& GetRootDir();
		bool SetRootDir( const std::string& path );
		bool IsMatchingRootDir( const std::string& path );

		const std::string& GetGameName();
		bool SetGame( const std::string& game_name );

		bool TryConstructTarget( const Proto::CsMessage& cs_msg, std::shared_ptr<LzsTarget>& source_target );
	private :
		std::string root_dir_;
		LzsGameList game_list_;
		LzsCurrentGame current_game_;

};

} //namespace SharedData
} //namespace Lazysplits