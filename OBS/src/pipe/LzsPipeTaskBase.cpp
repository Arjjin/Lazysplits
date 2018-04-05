#include "LzsPipeTask.h"

#include "obs.h"

namespace Lazysplits{

LzsPipeTaskBase::LzsPipeTaskBase( std::string owning_thread_name, HANDLE pipe_handle, LZS_PIPE_TASK_TYPE task_type, LZS_PIPE_STATE& pipe_server_state )
	:task_type_(task_type),pipe_server_state_(pipe_server_state)
{
	owning_thread_name_ = owning_thread_name;

	if( pipe_handle == NULL ){
		blog( LOG_WARNING, "[Lazysplits][%s] pipe task error : pipe handle is NULL in constructor", owning_thread_name_.c_str() );
		task_status_ = TASK_STATUS_ERROR;
		return;
	}
	pipe_handle_ = pipe_handle;

	memset( &task_overlapped_, 0, sizeof(OVERLAPPED) ); //make sure the members of our OVERLAPPED are set to 0

	task_event_handle_ = CreateEvent(
		NULL,													//default security attribute
		TRUE,													//manual-reset
		FALSE,													//initial state 
		NULL	//name
	);
	if( GetLastError() == ERROR_ALREADY_EXISTS ){
		blog( LOG_WARNING, "[Lazysplits][%s] pipe task error : pre existing handle returned!", owning_thread_name_.c_str() );
		task_status_ = TASK_STATUS_ERROR;
		return;
	}
	if( task_event_handle_ == NULL ){
		blog( LOG_WARNING, "[Lazysplits][%s] pipe task error : failed to create handle : %i", owning_thread_name_.c_str(), GetLastError() );
		task_status_ = TASK_STATUS_ERROR;
		return;
	}

	task_overlapped_.hEvent = task_event_handle_;
	task_status_ = TASK_STATUS_CREATED;
}

void LzsPipeTaskBase::HandleTaskResult(){
	if( task_event_handle_ != NULL){
		CloseHandle(task_event_handle_);
		task_event_handle_ = NULL;
		blog( LOG_DEBUG, "[Lazysplits][%s] pipe task handle closed", owning_thread_name_.c_str() );
	}
}

void LzsPipeTaskBase::CancelTask(){
	if( task_status_ == TASK_STATUS_RUNNING ){
		//BOOL cancel_result = CancelIoEx( pipe_handle_, &task_overlapped_ );
		blog( LOG_DEBUG, "[Lazysplits][%s] cancelling task", owning_thread_name_.c_str() );
		if( CancelIoEx( pipe_handle_, &task_overlapped_ ) == 0 ){
			DWORD last_error = GetLastError();
			if( last_error == ERROR_NOT_FOUND ){
				blog( LOG_WARNING, "[Lazysplits][%s] cancel task : could not find IO request to cancel!", owning_thread_name_.c_str() );
			}
			else{
				blog( LOG_WARNING, "[Lazysplits][%s] cancel task : unspecified error!", owning_thread_name_.c_str() );
			}
			task_status_ = TASK_STATUS_ERROR;
			return;
		}
		task_status_ = TASK_STATUS_CANCELLED;
	}
}

const LZS_PIPE_TASK_TYPE LzsPipeTaskBase::GetType(){ return task_type_; }

const LZS_PIPE_TASK_STATUS LzsPipeTaskBase::GetStatus(){ return task_status_; }

const HANDLE LzsPipeTaskBase::GetEventHandle(){ return task_event_handle_; }

} //namespace Lazysplits