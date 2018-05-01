#include "LzsPipeTask.h"
#include "cv\LzsCvThread.h"

#include "obs.h"

namespace Lazysplits {

LzsPipeTaskRead::LzsPipeTaskRead( std::string owning_thread_name, HANDLE pipe_handle, LZS_PIPE_STATE& pipe_server_state, int read_buf_size, LzsCvThread* cv_thread )
	:LzsPipeTaskBase( owning_thread_name, pipe_handle, TASK_TYPE_READ, pipe_server_state )
{
	read_buf_ = std::vector<byte>(read_buf_size);
	cv_thread_ = cv_thread;
}

void LzsPipeTaskRead::StartTask(){

	if( read_buf_.size() == 0 ){
		blog( LOG_WARNING, "[Lazysplits][%s] read task error : read buffer was created with size 0", owning_thread_name_.c_str() );
		task_status_ = TASK_STATUS_ERROR;
	}
	else{
		BOOL read_result = ReadFile(
			pipe_handle_,			//file handle
			read_buf_.data(),		//data buffer
			read_buf_.size(),		//max bytes to read
			NULL,					//num bytes read; not used with async I/O, get number of bytes read from GetOverlappedResult
			&task_overlapped_		//OVERLAPPED struct for async I/O
		);
		
		DWORD read_result_last_error = GetLastError();
		if( read_result == 0 ){
			if( read_result_last_error == ERROR_IO_PENDING ){
				blog( LOG_DEBUG, "[Lazysplits][%s] read task : waiting for data in pipe", owning_thread_name_.c_str() );
				task_status_ = TASK_STATUS_RUNNING;
			}
			else{
				blog( LOG_WARNING, "[Lazysplits][%s] read task : unknown error : %i", owning_thread_name_.c_str(), read_result_last_error );
				task_status_ = TASK_STATUS_ERROR;
			}
		}
		//sometimes async events can complete synchronously
		else{
			if( read_result_last_error == 0 ){
				blog( LOG_WARNING, "[Lazysplits][%s] read task returned != 0 trying to read from pipe (forced synchronous?)", owning_thread_name_.c_str() );
				//task_status_ = TASK_STATUS_COMPLETED;
				HandleTaskResult();
			}
			else{
				blog( LOG_WARNING, "[Lazysplits][%s] read task returned != 0 trying to read from pipe (forced synchronous?) : error %i", owning_thread_name_.c_str(), read_result_last_error );
				task_status_ = TASK_STATUS_ERROR;
			}
		}
	}
}


void LzsPipeTaskRead::HandleTaskResult(){
	DWORD bytes_transferred = 0;
	bool read_result = GetOverlappedResult( pipe_handle_, &task_overlapped_, &bytes_transferred, FALSE );
	if( read_result == 0 ){
		DWORD read_result_last_error = GetLastError();
		if( read_result_last_error == ERROR_BROKEN_PIPE ){
			blog( LOG_INFO, "[Lazysplits][%s] read task handling : pipe broken", owning_thread_name_.c_str() );
			task_status_ = TASK_STATUS_COMPLETED;
			pipe_server_state_ = PIPE_BROKEN;
		}
		else if( read_result_last_error == ERROR_OPERATION_ABORTED ){
			blog( LOG_DEBUG, "[Lazysplits][%s] read task handling : task cancelled", owning_thread_name_.c_str() );
			task_status_ = TASK_STATUS_CANCELLED;
		}
		else{
			blog( LOG_WARNING, "[Lazysplits][%s] read task handling failed : %i", owning_thread_name_.c_str(), read_result_last_error );
			task_status_ = TASK_STATUS_ERROR;
		}
	}
	else{
		//resize our vec to our data so we can get a proper terminal iterator
		read_buf_.resize(bytes_transferred);
		//make a string from our vec
		std::string message_string( read_buf_.begin(), read_buf_.end() );
		cv_thread_->MsgProtobuf(message_string);
		task_status_ = TASK_STATUS_COMPLETED;
	}

	LzsPipeTaskBase::HandleTaskResult();
}

} //namespace Lazysplits