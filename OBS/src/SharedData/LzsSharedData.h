#pragma once

#include "util\LzsCalibrationData.h"
#include "LzsCvDataProtoCpp.pb.h"
#include "LzsPipeProtoCpp.pb.h"

#include <opencv2\core.hpp>
#include <opencv2\core\types.hpp>
#include <google\protobuf\repeated_field.h>

#include <string>
#include <vector>
#include <memory>

namespace Lazysplits{
namespace SharedData{

enum LzsWatchStatus{ LZS_WATCH_ERROR, LZS_WATCH_UNITIALIZED, LZS_WATCH_GOOD };

class LzsWatchBase{
	public :
		LzsWatchBase(
			const Proto::WatchInfo& watch_info,
			const Proto::TargetInfo_WatchEntry& watch_entry,
			const std::string& watch_dir,
			const google::protobuf::RepeatedPtrField<Proto::CsMessage_WatchVariable>& watch_vars
		);

		bool IsGood();
		const std::string& GetName();
		Proto::WatchType GetType();
		int GetIndex();
		Proto::WatchAction GetAction();
		int GetActionVal();
		Proto::WatchPersistence GetPersistence();
		int GetPersistenceMax();
		float GetThreshold();

		bool FindWatch( const cv::Mat& BGR_frame, const SendableCalibrationProps& calib_props );
		virtual bool CvLogic( const cv::Mat& BGR_frame ) = 0;

	protected :
		void SetError();
		void SetUnitialized();
		void SetGood();

		const std::string& FindWatchVar( const std::string& var_name );

		void ValidateData( int source_width, int source_height, const SendableCalibrationProps& calib_props );
		bool ShouldRemakeData( int source_width, int source_height, const SendableCalibrationProps& calib_props );
		virtual bool RemakeData() = 0;
		
		virtual bool MakeArea();
		virtual bool CheckBounds();
		bool IsDiffCalibProps( const SendableCalibrationProps& calib_props );

		Proto::WatchInfo watch_info_;
		LzsWatchStatus status_;
		int index_;
		Proto::WatchAction action_;
		int action_val_;
		Proto::WatchPersistence persistence_;
		int persistence_max_;
		std::string watch_dir_;
		std::unordered_map<std::string,std::string> watch_vars_;

		cv::Rect area_;

		SendableCalibrationProps current_calib_props_;
		int current_source_width_;
		int current_source_height_;
};

class LzsWatchColor : public LzsWatchBase{
	public :
		LzsWatchColor(
			const Proto::WatchInfo& watch_info,
			const Proto::TargetInfo_WatchEntry& watch_entry,
			const std::string& watch_dir,
			const google::protobuf::RepeatedPtrField<Proto::CsMessage_WatchVariable>& watch_vars
		);
		bool CvLogic( const cv::Mat& BGR_frame )override;
	private :
		//bool MakeArea()override;
		//bool CheckBounds()override;
		bool RemakeData()override;

		cv::Scalar RGB_scalar_;
};

class LzsWatchImageBase : public LzsWatchBase{
	public :
		LzsWatchImageBase(
			const Proto::WatchInfo& watch_info,
			const Proto::TargetInfo_WatchEntry& watch_entry,
			const std::string& watch_dir,
			const google::protobuf::RepeatedPtrField<Proto::CsMessage_WatchVariable>& watch_vars
		);
	protected :
		virtual bool MakeImage();
		virtual bool CheckBounds()override;

		cv::Mat img_BGR_;
		cv::Mat img_mask_;
};

class LzsWatchImageStatic : public LzsWatchImageBase{
	public :
		LzsWatchImageStatic(
			const Proto::WatchInfo& watch_info,
			const Proto::TargetInfo_WatchEntry& watch_entry,
			const std::string& watch_dir,
			const google::protobuf::RepeatedPtrField<Proto::CsMessage_WatchVariable>& watch_vars
		);
		bool CvLogic( const cv::Mat& BGR_frame )override;
	private :
		bool RemakeData()override;
};

class LzsWatchCharacterSet : public LzsWatchImageBase{
	public :
		LzsWatchCharacterSet(
			const Proto::WatchInfo& watch_info,
			const Proto::TargetInfo_WatchEntry& watch_entry,
			const std::string& watch_dir,
			const google::protobuf::RepeatedPtrField<Proto::CsMessage_WatchVariable>& watch_vars
		);
		bool CvLogic( const cv::Mat& BGR_frame )override;
	private :
		bool MakeArea()override;
		bool MakeImage()override;
		bool RemakeData()override;

		void MakeCharMap();
		void MakeCharInput( const std::string& input_string );
		cv::Mat GetCharMat( cv::Mat source_mat, int char_index, int char_width, int char_height );
		const cv::Point2i GetAdditonalAreaOffset();

		//const std::string character_input_;
		std::unordered_map<char,Proto::WatchInfo_CharacterEntry> character_map_;
		std::vector<Proto::WatchInfo_CharacterEntry> character_input_;

};

class LzsTarget{
	public :
		LzsTarget( const std::string& game_info_dir, const Proto::TargetInfo& target_info );
		bool ParseWatchList( const google::protobuf::RepeatedPtrField<Proto::CsMessage_WatchVariable>& watch_vars_it );

		const std::string& GetName();
		Proto::TargetType GetType();
		uint64_t GetSplitOffset();
		const std::vector<std::shared_ptr<LzsWatchBase>> GetCurrentWatches();
		void WatchAction( Proto::WatchAction action, int action_val );
		bool IsComplete();
	private :

		Proto::TargetInfo target_info_;
		std::string game_info_dir_;
		std::vector<std::shared_ptr<LzsWatchBase>> constant_watches_;
		std::vector<std::shared_ptr<LzsWatchBase>> watches_;
		uint32_t current_watch_index_;
		bool is_complete_;
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
		
		bool TryConstructTargetList( const Proto::CsMessage& cs_msg, std::vector<std::shared_ptr<SharedData::LzsTarget>>& target_list );
	private :
		std::string root_dir_;
		LzsGameList game_list_;
		LzsCurrentGame current_game_;

};

} //namespace SharedData
} //namespace Lazysplits