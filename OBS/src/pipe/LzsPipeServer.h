#pragma once

#include "..\util\LzsThread.h"
#include "..\util\LzsObserver.h"
#include "LzsPipeConstants.h"
#include "LzsPipeTask.h"
#include "LzsMessageQueue.h"

#include <Windows.h>

namespace Lazysplits{

struct LzsPipeParams{
	std::string			  name;
	DWORD				  open_mode;
	DWORD				  pipe_mode;
	DWORD				  max_instances;
	DWORD				  out_buffer_size;
	DWORD				  in_buffer_size;
	DWORD				  default_timeout;
	LPSECURITY_ATTRIBUTES security_attributes;
};

class LzsPipeServer : public LzsThread, public LzsObserver {
	public :
		LzsPipeServer( std::string pipe_name, DWORD buffer_size, LzsMessageQueue<std::string>* cv_to_pipe_queue, LzsMessageQueue<std::string>* pipe_to_cv_queue );
		void ThreadFuncInit()override;
		void* ThreadFunc()override;
		void ThreadFuncCleanup()override;

		void ThreadTerminate()override;

		//observer event from message queue
		void OnSubjectNotify()override;
	private :
		void CreatePipe();
		void CheckWriteQueue();
		
		HANDLE pipe_handle_;
		LzsPipeParams pipe_params_;
		LZS_PIPE_STATE pipe_state_;

		std::shared_ptr<LzsPipeTaskManager> pipe_task_manager_;

		//message queue stuff
		LzsMessageQueue<std::string>* cv_to_pipe_queue_;
		LzsMessageQueue<std::string>* pipe_to_cv_queue_;
};

} //namespace Lazysplits