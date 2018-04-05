#pragma once

#include <string>
#include <vector>
#include <memory>
#include <Windows.h>

#include "LzsPipeConstants.h"
#include "LzsMessageQueue.h"

namespace Lazysplits{

class LzsPipeTaskBase{
	public :
		LzsPipeTaskBase( std::string owning_thread_name, HANDLE pipe_handle, LZS_PIPE_TASK_TYPE task_type, LZS_PIPE_STATE& pipe_server_state );
		virtual void StartTask() = 0;
		virtual void HandleTaskResult();
		void CancelTask();

		const LZS_PIPE_TASK_TYPE GetType();
		const LZS_PIPE_TASK_STATUS GetStatus();
		const HANDLE GetEventHandle();

	protected :
		HANDLE pipe_handle_;
		LZS_PIPE_STATE& pipe_server_state_;
		std::string owning_thread_name_;

		//win event stuff
		OVERLAPPED task_overlapped_;
		HANDLE task_event_handle_;

		const LZS_PIPE_TASK_TYPE task_type_;
		LZS_PIPE_TASK_STATUS task_status_;
};

class LzsPipeTaskConnect : public LzsPipeTaskBase{
	public :
		LzsPipeTaskConnect( std::string owning_thread_name, HANDLE pipe_handle, LZS_PIPE_STATE& pipe_server_state );
		void StartTask()override;
		void HandleTaskResult()override;
};

class LzsPipeTaskRead : public LzsPipeTaskBase{
	public :
		LzsPipeTaskRead( std::string owning_thread_name, HANDLE pipe_handle, LZS_PIPE_STATE& pipe_server_state, int read_buf_size, LzsMessageQueue<std::string>* read_queue );
		void StartTask()override;
		void HandleTaskResult()override;
	private :
		//use vector for a safe runtime sized array, and pass it as a raw array with data()
		std::vector<byte> read_buf_;
		LzsMessageQueue<std::string>* read_queue_;
};

class LzsPipeTaskWrite : public LzsPipeTaskBase{
	public :
		LzsPipeTaskWrite( std::string owning_thread_name, HANDLE pipe_handle, LZS_PIPE_STATE& pipe_server_state, std::string serialized_protobuf );
		void StartTask()override;
		void HandleTaskResult()override;
	private :
		std::string serialized_protobuf_;
};

class LzsPipeTaskManager{
	public :
		LzsPipeTaskManager( std::string owning_thread_name, HANDLE pipe_handle, LZS_PIPE_STATE& pipe_server_state );
		~LzsPipeTaskManager();
		
		void AddConnectTask();
		void AddReadTask( int read_buf_size, LzsMessageQueue<std::string>* read_queue );
		void AddWriteTask( std::string serialized_protobuf );
		void StartTasks();
		void WaitOnTasks();
		bool IsWaiting();
		void CancelWait();
		void RemoveStrayTasks();
		void CancelTasks();

		bool IsTaskInList( LZS_PIPE_TASK_TYPE task_type );
	private :
		int GetTaskIndex( HANDLE task_handle );

		HANDLE pipe_handle_;
		LZS_PIPE_STATE& pipe_server_state_;
		std::string owning_thread_name_;

		std::vector<std::shared_ptr<LzsPipeTaskBase>> task_list_;
		
		HANDLE wait_cancellation_handle_;
		bool is_waiting_;
};

} //namespace Lazysplits