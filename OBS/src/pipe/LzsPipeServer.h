#pragma once

#include "LzsPipeTask.h"
#include "util\LzsThread.h"
#include "util\LzsObserver.h"
#include "LzsPipeConstants.h"
#include "LzsMessageQueue.h"
#include "cv\LzsCvThread.h"

#include <memory>
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

//forward declaration for circular dependency
class LzsCvThread;

class LzsPipeServer : public LzsThread, public LzsObserver{
	public :
		LzsPipeServer( std::string pipe_name, DWORD buffer_size );
		void AssignCvThread( LzsCvThread* cv_thread );

		void ThreadTerminate()override;
		bool IsConnected();

		void OnSubjectNotify( const std::string& subject_name, const std::string& subject_message );
	private :
		void CreatePipe();
		void CheckWriteQueue();

		void ThreadFuncInit()override;
		void* ThreadFunc()override;
		void ThreadFuncCleanup()override;
		
		HANDLE pipe_handle_;
		LzsPipeParams pipe_params_;
		LZS_PIPE_STATE pipe_state_;

		std::shared_ptr<LzsPipeTaskManager> pipe_task_manager_;

		//pointer to cv thread
		LzsCvThread* cv_thread_;

		//message queue stuff
		LzsMsgQueue<std::string> msg_queue_;
};

} //namespace Lazysplits