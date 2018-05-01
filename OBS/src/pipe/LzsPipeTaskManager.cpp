#include "LzsPipeTask.h" 

#include <algorithm>

#include "obs.h"

namespace Lazysplits{


LzsPipeTaskManager::LzsPipeTaskManager( std::string owning_thread_name, HANDLE pipe_handle, LZS_PIPE_STATE& pipe_server_state )
	:pipe_server_state_(pipe_server_state)
{
	owning_thread_name_ = owning_thread_name;
	pipe_handle_ = pipe_handle;

	wait_cancellation_handle_ = CreateEvent(
		NULL,						//default security attribute
		TRUE,						//manual-reset
		FALSE,						//initial state 
		NULL	//name
	);
	if( wait_cancellation_handle_ == NULL ){
		blog( LOG_ERROR, "[Lazysplits][%s] pipe task manager failed to create cancellation handle : %i", owning_thread_name_.c_str(), GetLastError() );
	}

	is_waiting_ = false;
}

LzsPipeTaskManager::~LzsPipeTaskManager(){
	if( wait_cancellation_handle_ != NULL ){
		CloseHandle(wait_cancellation_handle_);
	}
}

void LzsPipeTaskManager::AddConnectTask(){
	std::shared_ptr<LzsPipeTaskConnect> connect_task = std::make_shared<LzsPipeTaskConnect>( owning_thread_name_, pipe_handle_, pipe_server_state_ );
	task_list_.push_back(connect_task);
}

void LzsPipeTaskManager::AddReadTask( int read_buf_size, LzsCvThread* cv_thread ){
	std::shared_ptr<LzsPipeTaskRead> read_task = std::make_shared<LzsPipeTaskRead>( owning_thread_name_, pipe_handle_, pipe_server_state_, read_buf_size, cv_thread );
	task_list_.push_back(read_task);
}

void LzsPipeTaskManager::AddWriteTask( std::string serialized_protobuf ){
	std::shared_ptr<LzsPipeTaskWrite> write_task = std::make_shared<LzsPipeTaskWrite>( owning_thread_name_, pipe_handle_, pipe_server_state_, serialized_protobuf );
	task_list_.push_back(write_task);
}

void LzsPipeTaskManager::StartTasks(){
	for( auto task_it = task_list_.begin(); task_it != task_list_.end(); ++task_it ){
		if( task_it->get()->GetStatus() == TASK_STATUS_CREATED ){ task_it->get()->StartTask(); }
	}
}

void LzsPipeTaskManager::WaitOnTasks(){
	if( wait_cancellation_handle_ == NULL ){
		blog( LOG_ERROR, "[Lazysplits][%s] pipe task manager cancellation handle is null!", owning_thread_name_.c_str() );
		return;
	}

	std::vector<HANDLE> handle_vec;
	//add handles only from tasks that are running
	for( auto task_it = task_list_.begin(); task_it != task_list_.end(); ++task_it ){
		if( task_it->get()->GetStatus() == TASK_STATUS_RUNNING ){
			handle_vec.push_back( task_it->get()->GetEventHandle() );
		}
	}

	if( handle_vec.size() > 0 ){
		handle_vec.push_back(wait_cancellation_handle_);

		//blog( LOG_DEBUG, "[Lazysplits][%s] waiting on %i tasks", owning_thread_name_.c_str(), handle_vec.size() );
		is_waiting_ = true;
		//timeout should be infinite instead of 2000ms, but a longish one prevents write queue race conditions from sloppy code, and shouldn't hit processor that hard
		DWORD wait_result = WaitForMultipleObjects( handle_vec.size(), handle_vec.data(), FALSE, 2000 );
		is_waiting_ = false;
		
		if( wait_result == WAIT_FAILED ){
			blog( LOG_WARNING, "[Lazysplits][%s] task wait failed : %i", owning_thread_name_.c_str(), GetLastError() );
		}
		else if( wait_result == WAIT_TIMEOUT ){
			//blog( LOG_DEBUG, "[Lazysplits][%s] task wait timed out", owning_thread_name_.c_str() );
		}
		else if( handle_vec.at(wait_result) == wait_cancellation_handle_ ){
			blog( LOG_DEBUG, "[Lazysplits][%s] task wait cancelled", owning_thread_name_.c_str() );
			ResetEvent(wait_cancellation_handle_);
		}
		else{
			int task_index = GetTaskIndex( handle_vec.at(wait_result) );

			if( task_index == -1 ){
				blog( LOG_DEBUG, "[Lazysplits][%s] failed to find task index!", owning_thread_name_.c_str() );
			}
			else{
				auto task_it = task_list_.at(task_index);
				//only handle result if task is running and not cancelled/faulty
				if( task_it->GetStatus() == TASK_STATUS_RUNNING ){ task_it->HandleTaskResult(); }
			}
		}
	}
}

bool LzsPipeTaskManager::IsWaiting(){ return is_waiting_; }

void LzsPipeTaskManager::CancelWait(){ if( is_waiting_ ){ SetEvent(wait_cancellation_handle_); } }

void LzsPipeTaskManager::RemoveStrayTasks(){
	//remove tasks in task list that are completed, cancelled or faulty
	//cool, readable syntax
	task_list_.erase(
		std::remove_if(
			task_list_.begin(),
			task_list_.end(),
			[]( std::shared_ptr<LzsPipeTaskBase>& task ){
				LZS_PIPE_TASK_STATUS task_status = task->GetStatus();
				if( task_status == TASK_STATUS_COMPLETED || task_status == TASK_STATUS_CANCELLED || task_status == TASK_STATUS_ERROR ){
					blog( LOG_DEBUG, "[Lazysplits][lambda] task of type %i removed(status : %i)", task->GetType(), task_status );
					return true;
				}
				return false;
			}
		),
		task_list_.end()
	);
}

void LzsPipeTaskManager::CancelTasks(){
	for( auto task_it = task_list_.begin(); task_it != task_list_.end(); ++task_it ){
		LZS_PIPE_TASK_STATUS task_status = task_it->get()->GetStatus();
		if( task_status == TASK_STATUS_CREATED || task_status == TASK_STATUS_RUNNING ){ task_it->get()->CancelTask(); }
		//task_it->get()->CancelTask();
	}
}

bool LzsPipeTaskManager::IsTaskInList( LZS_PIPE_TASK_TYPE task_type ){
	for( auto task_it = task_list_.begin(); task_it != task_list_.end(); ++task_it ){
		if( task_it->get()->GetType() == task_type ){ return true; }
	}
	return false;
}

int LzsPipeTaskManager::GetTaskIndex( HANDLE task_handle ){
	int found_index = -1;
	for( auto task_it = task_list_.begin(); task_it != task_list_.end(); ++task_it ){
		if( task_it->get()->GetEventHandle() == task_handle ){
			//get index from iterator
			found_index = task_it - task_list_.begin();
			break;
		}
	}

	return found_index;
}

} //namespace Lazysplits