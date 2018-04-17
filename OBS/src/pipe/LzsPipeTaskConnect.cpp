#include "LzsPipeTask.h"

#include "obs.h"

namespace Lazysplits{

LzsPipeTaskConnect::LzsPipeTaskConnect( std::string owning_thread_name, HANDLE pipe_handle, LZS_PIPE_STATE& pipe_server_state )
	:LzsPipeTaskBase( owning_thread_name, pipe_handle, TASK_TYPE_CONNECT, pipe_server_state )
{
}

void LzsPipeTaskConnect::StartTask(){
	if( task_status_ == TASK_STATUS_CREATED ){
		if(  ConnectNamedPipe( pipe_handle_, &task_overlapped_ ) == 0 ){
			DWORD connect_result_last_error = GetLastError();

			if( connect_result_last_error == ERROR_IO_PENDING ){
				blog( LOG_DEBUG, "[Lazysplits][%s] connection task : waiting for client connection", owning_thread_name_.c_str() );
				task_status_ = TASK_STATUS_RUNNING;
			}
			//weird edge case, can't wait on this tasks overlapped ev/handle in our task manager wait function since it's already complete, so just call HandleTaskResult from here
			else if( connect_result_last_error == ERROR_PIPE_CONNECTED ){
				blog( LOG_INFO, "[Lazysplits][%s] connection task : client already connected", owning_thread_name_.c_str() );
				//task_status_ = TASK_STATUS_COMPLETED;
				HandleTaskResult();
			}
			else{
				blog( LOG_WARNING, "[Lazysplits][%s] connection task : unknown error : %i", owning_thread_name_.c_str(), connect_result_last_error );
				task_status_ = TASK_STATUS_ERROR;
			}
		}
		//sometimes async events can complete synchronously
		else{
			DWORD connect_result_last_error = GetLastError();
			if( connect_result_last_error == 0 ){
				blog( LOG_WARNING, "[Lazysplits][%s] task returned != 0 trying to connect to pipe (forced synchronous?)", owning_thread_name_.c_str() );
				//task_status_ = TASK_STATUS_COMPLETED;
				HandleTaskResult();
			}
			else{
				blog( LOG_WARNING, "[Lazysplits][%s] task returned != 0 trying to connect to pipe (forced synchronous?) : error %i", owning_thread_name_.c_str(), GetLastError() );
				task_status_ = TASK_STATUS_ERROR;
			}
		}
	}
}

void LzsPipeTaskConnect::HandleTaskResult(){
	pipe_server_state_ = PIPE_CONNECTED;
	task_status_ = TASK_STATUS_COMPLETED;
	LzsPipeTaskBase::HandleTaskResult();
}

} //namespace Lazysplits