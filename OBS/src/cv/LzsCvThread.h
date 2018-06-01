
#pragma once

#include "util\LzsThread.h"
#include "util\LzsObserver.h"
#include "util\LzsFrameBuffer.h"
#include "util\LzsCalibrationData.h"
#include "pipe\LzsPipeServer.h"
#include "pipe\LzsMessageQueue.h"
#include "pipe\LzsMessage.h"
#include "SharedData\LzsSharedData.h"

#include "LzsPipeProtoCpp.pb.h"

#include <atomic>
#include <memory>

namespace Lazysplits{

//forward declaration for circular dependency
class LzsPipeServer;

//main thread class
class LzsCvThread : public LzsThread, public LzsObserver{
	public :
		LzsCvThread( LzsFrameBuffer* frame_buf, obs_source_t* context );
		void AssignPipe( LzsPipeServer* pipe );
		
		bool IsTargets();

		//message queue helpers
		void MsgPipeConnected( bool connected );
		void MsgSetSharedDataPath( const std::string& path );
		void MsgProtobuf( std::string serialized_protobuf );
		void MsgCalibData( const SendableCalibrationProps& calib_props );

		void OnSubjectNotify( const std::string& subject_name, const std::string& subject_message )override;
	private :
		void ThreadFuncInit()override;
		void* ThreadFunc()override;
		void ThreadFuncCleanup()override;

		void HandleFrameBuffer();
		
		//pipe to livesplit message queue helpers
		int GetLsMsgId();
		void LsMsgRequestResync();
		void LsMsgTargetFound( const std::string& game_name, const std::string& target_name, uint64_t timestamp, uint64_t split_offset );

		//message queue handling
		void HandleMessageQueue();
		void HandlePipeConnected( std::shared_ptr<CvMsg> msg );
		void HandleSetSharedDataPath( std::shared_ptr<CvMsg> msg );
		void HandleProtobuf( std::shared_ptr<CvMsg> msg );
		void HandleCalibrationData( std::shared_ptr<CvMsg> msg );

		void NewTargets( Proto::CsMessage& msg );

		obs_source_t* context_;

		LzsFrameBuffer* frame_buf_;

		LzsPipeServer* pipe_;
		bool pipe_connected_;
		
		SharedData::LzsSharedDataManager shared_data_manager_;
		std::vector<std::shared_ptr<SharedData::LzsTarget>> target_list_;

		LzsMsgQueue<std::shared_ptr<CvMsg>> msg_queue_;
		int32_t ls_msg_id_;

		SendableCalibrationProps calib_props_;
};

} //namespace Lazysplits