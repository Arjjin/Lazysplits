#pragma once

#include "../util/LzsThread.h"
#include "../util/LzsObserver.h"
#include "../util/LzsFrameBuffer.h"
#include "../pipe/LzsMessageQueue.h"

namespace Lazysplits{

class LzsCvThread : public LzsThread, public LzsObserver{
	public :
		LzsCvThread( LzsFrameBuffer* frame_buf, LzsMessageQueue<std::string>* cv_to_pipe_queue, LzsMessageQueue<std::string>* pipe_to_cv_queue );

		void OnSubjectNotify( std::string subject_name )override;
		void* ThreadFunc()override;
		void ThreadFuncInit()override;
		void ThreadFuncCleanup()override;
	private :
		void HandleMessages();
		void HandleFrameBuffer();

		LzsFrameBuffer* frame_buf_;

		//message queue stuff
		LzsMessageQueue<std::string>* cv_to_pipe_queue_;
		LzsMessageQueue<std::string>* pipe_to_cv_queue_;
		bool should_read_queue_;
};

} //namespace Lazysplits