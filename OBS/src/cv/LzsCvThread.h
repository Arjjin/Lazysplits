#pragma once

#include "util/LzsThread.h"
#include "util/LzsObserver.h"
#include "util/LzsFrameBuffer.h"
#include "pipe/LzsMessageQueue.h"

#include <atomic>

namespace Lazysplits{

class LzsCvThread : public LzsThread, public LzsObserver{
	public :
		LzsCvThread( LzsFrameBuffer* frame_buf, LzsMessageQueue<std::string>* cv_to_pipe_queue, LzsMessageQueue<std::string>* pipe_to_cv_queue );
		
		bool IsCvActive();

		void OnSubjectNotify( std::string subject_name, std::string subject_message )override;
	private :
		void ThreadFuncInit()override;
		void* ThreadFunc()override;
		void ThreadFuncCleanup()override;

		void ActivateCv();
		void DeactivateCv();
		void HandleMessages();
		void HandleFrameBuffer();

		LzsFrameBuffer* frame_buf_;

		//message queue stuff
		LzsMessageQueue<std::string>* cv_to_pipe_queue_;
		LzsMessageQueue<std::string>* pipe_to_cv_queue_;
		bool should_read_queue_;
		std::atomic_bool is_cv_active_;
		int32_t message_id_ref_;
};

} //namespace Lazysplits