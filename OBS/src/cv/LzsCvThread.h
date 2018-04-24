#pragma once

#include "util\LzsThread.h"
#include "pipe\LzsPipeServer.h"
#include "util\LzsObserver.h"
#include "util\LzsFrameBuffer.h"
#include "pipe\LzsMessageQueue.h"
#include "pipe\LzsMessage.h"

#include <atomic>
#include <memory>

namespace Lazysplits{

//forward declaration for circular dependency
class LzsPipeServer;

//main thread class
class LzsCvThread : public LzsThread, public LzsObserver{
	public :
		LzsCvThread( LzsFrameBuffer* frame_buf );
		void AssignPipe( LzsPipeServer* pipe );
		
		bool IsCvActive();

		//message queue helpers
		void MsgPipeConnected( bool connected );
		void MsgSetSharedDataPath( std::string path );
		void MsgProtobuf( std::string serialized_protobuf );

		void OnSubjectNotify( const std::string& subject_name, const std::string& subject_message )override;
	private :
		void ThreadFuncInit()override;
		void* ThreadFunc()override;
		void ThreadFuncCleanup()override;

		void ActivateCv();
		void DeactivateCv();
		void HandleFrameBuffer();

		//message queue handling
		void HandleMessageQueue();
		void HandlePipeConnected( std::shared_ptr<CvMsg> msg );
		void HandleSetSharedDataPath( std::shared_ptr<CvMsg> msg );
		void HandleProtobuf( std::shared_ptr<CvMsg> msg );

		//pointer to pipe thread
		LzsPipeServer* pipe_;

		LzsFrameBuffer* frame_buf_;

		std::atomic_bool is_cv_active_;
		std::string shared_data_dir_;

		//message queue stuff
		LzsMsgQueue<std::shared_ptr<CvMsg>> msg_queue_;
		int32_t pipe_message_id_ref_;
};

} //namespace Lazysplits