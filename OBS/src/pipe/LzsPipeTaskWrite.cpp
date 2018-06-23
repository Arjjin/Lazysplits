#include "LzsPipeTask.h"

#include "obs.h"

namespace Lazysplits{

LzsPipeTaskWrite::LzsPipeTaskWrite( std::string owning_thread_name, HANDLE pipe_handle, LZS_PIPE_STATE& pipe_server_state, std::string serialized_protobuf )
	:LzsPipeTaskBase( owning_thread_name, pipe_handle, TASK_TYPE_WRITE, pipe_server_state )
{
	serialized_protobuf_ = serialized_protobuf;
}

void LzsPipeTaskWrite::StartTask(){
	if( serialized_protobuf_.size() == 0 ){
		blog( LOG_WARNING, "[Lazysplits][%s] write task error : serialized protobuf is size 0", owning_thread_name_.c_str() );
		task_status_ = TASK_STATUS_ERROR;
	}
	else{
		BOOL write_result = WriteFile(
			pipe_handle_,							//file handle
			serialized_protobuf_.data(),			//data buffer
			serialized_protobuf_.size(),			//num bytes to write
			NULL,									//num bytes written; not used with async I/O, get number of bytes read from GetOverlappedResult
			&task_overlapped_						//OVERLAPPED struct for async I/O
		);
		
		DWORD write_result_last_error = GetLastError();
		if( write_result == 0 ){
			if( write_result_last_error == ERROR_IO_PENDING ){
				blog( LOG_DEBUG, "[Lazysplits][%s] write task : waiting for write to complete", owning_thread_name_.c_str() );
				task_status_ = TASK_STATUS_RUNNING;
			}
			else{
				blog( LOG_WARNING, "[Lazysplits][%s] write task : unknown error : %i", owning_thread_name_.c_str(), write_result_last_error );
				task_status_ = TASK_STATUS_ERROR;
			}
		}
		//sometimes async events can complete synchronously
		else{
			if( write_result_last_error == 0 ){
				blog( LOG_DEBUG, "[Lazysplits][%s] write task returned != 0 trying to write to pipe (forced synchronous?)", owning_thread_name_.c_str() );
				//task_status_ = TASK_STATUS_COMPLETED;
				HandleTaskResult();
			}
			else if( write_result_last_error == ERROR_IO_PENDING ){
				blog( LOG_DEBUG, "[Lazysplits][%s] write task : waiting for write to complete", owning_thread_name_.c_str() );
				task_status_ = TASK_STATUS_RUNNING;
			}
			else{
				blog( LOG_WARNING, "[Lazysplits][%s] write task returned != 0 trying to write to pipe (forced synchronous?) : error %i", owning_thread_name_.c_str(), write_result_last_error );
				task_status_ = TASK_STATUS_ERROR;
			}
		}
	}
}

void LzsPipeTaskWrite::HandleTaskResult(){
	DWORD bytes_transferred = 0;
	bool write_result = GetOverlappedResult( pipe_handle_, &task_overlapped_, &bytes_transferred, FALSE );
	if( write_result == 0 ){
		DWORD write_result_last_error = GetLastError();
		if( write_result_last_error == ERROR_BROKEN_PIPE ){
			blog( LOG_INFO, "[Lazysplits][%s] write task handling : pipe broken", owning_thread_name_.c_str() );
			task_status_ = TASK_STATUS_COMPLETED;
			pipe_server_state_ = PIPE_BROKEN;
		}
		else if( write_result_last_error == ERROR_OPERATION_ABORTED ){
			blog( LOG_DEBUG, "[Lazysplits][%s] write task handling : task cancelled", owning_thread_name_.c_str() );
			task_status_ = TASK_STATUS_CANCELLED;
		}
		else{
			blog( LOG_WARNING, "[Lazysplits][%s] write task handling failed : %i", owning_thread_name_.c_str(), write_result_last_error );
			task_status_ = TASK_STATUS_ERROR;
		}
	}
	else{
		blog( LOG_DEBUG, "[Lazysplits][%s] write task handled : %i bytes transferred", owning_thread_name_.c_str(), bytes_transferred );
		task_status_ = TASK_STATUS_COMPLETED;
	}
}

} //namespace Lazysplits