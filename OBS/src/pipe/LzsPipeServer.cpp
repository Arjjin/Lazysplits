#include "LzsPipeServer.h"

#include <sstream>

namespace Lazysplits{

LzsPipeServer::LzsPipeServer( std::string pipe_name, DWORD buffer_size, LzsMessageQueue<std::string>* cv_to_pipe_queue, LzsMessageQueue<std::string>* pipe_to_cv_queue  )
	:LzsThread("Pipe server thread")
{
	pipe_params_.name = pipe_name;
	pipe_params_.open_mode = PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED; // duplex directional pipe, function will fail if pipe already exists, using overlapped IO
	pipe_params_.pipe_mode = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE;
	pipe_params_.max_instances = 1;
	pipe_params_.out_buffer_size = buffer_size;
	pipe_params_.in_buffer_size = buffer_size;
	pipe_params_.default_timeout = 50000; //defaults to 50ms
	pipe_params_.security_attributes = NULL; // security attributes (NULL for default)

	cv_to_pipe_queue_ = cv_to_pipe_queue;
	pipe_to_cv_queue_ = pipe_to_cv_queue;
	cv_to_pipe_queue_->AttachObserver(this);

	pipe_state_ = PIPE_NOT_CREATED;
}

void LzsPipeServer::ThreadFuncInit(){
	LzsThread::ThreadFuncInit();

	CreatePipe();
}

void* LzsPipeServer::ThreadFunc(){
	ThreadFuncInit();

	while( ThreadFuncShouldLoop() ){
		switch(pipe_state_){
			case PIPE_CREATED :
				if( !pipe_task_manager_->IsTaskInList(TASK_TYPE_CONNECT) ){ pipe_task_manager_->AddConnectTask(); }
			break;
			case PIPE_CONNECTED :
				//allow only one read task at a time
				if( !pipe_task_manager_->IsTaskInList(TASK_TYPE_READ) ){ pipe_task_manager_->AddReadTask( pipe_params_.in_buffer_size, pipe_to_cv_queue_ ); }
				//if our cv_to_pipe queue has messages for us to write, do so
				CheckWriteQueue();
			break;
			case PIPE_BROKEN :
				if( pipe_handle_ != NULL ){
					CloseHandle(pipe_handle_);
					pipe_handle_ = NULL;
					blog( LOG_DEBUG, "[Lazysplits][%s] Pipe handle closed" , thread_name_.c_str());
				}

				CreatePipe();
			break;
		}
		pipe_task_manager_->StartTasks();
		pipe_task_manager_->WaitOnTasks();
		pipe_task_manager_->RemoveStrayTasks();
	}

	ThreadFuncCleanup();
	return NULL;
}

void LzsPipeServer::ThreadFuncCleanup(){
	if( pipe_handle_ != NULL ){
		CloseHandle(pipe_handle_);
		pipe_handle_ = NULL;
		blog( LOG_DEBUG, "[Lazysplits][%s] Pipe handle closed" , thread_name_.c_str());

	}
	pipe_state_ = PIPE_NOT_CREATED;

	LzsThread::ThreadFuncCleanup();
}

void LzsPipeServer::ThreadTerminate(){
	if( pipe_state_ >= PIPE_CREATED ){
		if( pipe_task_manager_->IsWaiting() ){ pipe_task_manager_->CancelTasks(); }
	}

	LzsThread::ThreadTerminate();
}

void LzsPipeServer::OnSubjectNotify(){
	if( pipe_task_manager_->IsWaiting() ){ pipe_task_manager_->CancelWait(); }
}

void LzsPipeServer::CreatePipe(){
	std::stringstream full_pipe_name;
	full_pipe_name << "\\\\.\\pipe\\" << pipe_params_.name;

	pipe_handle_ = CreateNamedPipe(
		static_cast<LPCSTR>( full_pipe_name.str().c_str() ),
		pipe_params_.open_mode,
		pipe_params_.pipe_mode,
		pipe_params_.max_instances,
		pipe_params_.out_buffer_size,
		pipe_params_.in_buffer_size,
		pipe_params_.default_timeout,
		pipe_params_.security_attributes
	);

    if ( pipe_handle_ == NULL ){
		blog( LOG_ERROR,  "[Lazysplits][%s] Failed to create named pipe : handle is NULL", thread_name_.c_str(), GetLastError() );
		pipe_state_ = PIPE_ERROR;
		ThreadTerminate();
		return;
	}
	else if( pipe_handle_ == INVALID_HANDLE_VALUE) {
		blog( LOG_ERROR,  "[Lazysplits][%s] Failed to create named pipe : %i", thread_name_.c_str(), GetLastError() );
		pipe_state_ = PIPE_ERROR;
		ThreadTerminate();
		return;
    }
	
	pipe_task_manager_ = std::make_shared<LzsPipeTaskManager>( thread_name_, pipe_handle_, pipe_state_ );
	blog( LOG_INFO,  "[Lazysplits][%s] Pipe created", thread_name_.c_str() );
	pipe_state_ = PIPE_CREATED;
}

void LzsPipeServer::CheckWriteQueue(){
	while( !cv_to_pipe_queue_->QueueIsEmpty() ){
		pipe_task_manager_->AddWriteTask( cv_to_pipe_queue_->QueueFront() );
		cv_to_pipe_queue_->QueuePop();
	}
}

} //namespace Lazysplits