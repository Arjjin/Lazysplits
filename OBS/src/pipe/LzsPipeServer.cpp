#include "LzsPipeServer.h"

#include <sstream>

namespace Lazysplits{

LzsPipeServer::LzsPipeServer( std::string pipe_name, DWORD buffer_size )
	:LzsThread("Pipe server thread"),
	 msg_queue_("Pipe queue")
{
	pipe_params_.name = pipe_name;
	pipe_params_.open_mode = PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED; // duplex directional pipe, function will fail if pipe already exists, using overlapped IO
	pipe_params_.pipe_mode = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE;
	pipe_params_.max_instances = 1;
	pipe_params_.out_buffer_size = buffer_size;
	pipe_params_.in_buffer_size = buffer_size;
	pipe_params_.default_timeout = 50000; //defaults to 50ms
	pipe_params_.security_attributes = NULL; // security attributes (NULL for default)

	pipe_state_ = PIPE_NOT_CREATED;
}

void LzsPipeServer::AssignCvThread( LzsCvThread* cv_thread ){ cv_thread_ = cv_thread; }

void LzsPipeServer::ThreadFuncInit(){
	LzsThread::ThreadFuncInit();
	
	msg_queue_.AttachObserver(this);
	CreatePipe();
}

void* LzsPipeServer::ThreadFunc(){
	ThreadFuncInit();

	LZS_PIPE_STATE state_last_loop = pipe_state_;
	while( ThreadFuncShouldLoop() ){
		switch(pipe_state_){
			case PIPE_CREATED :
				if( !pipe_task_manager_->IsTaskInList(TASK_TYPE_CONNECT) ){ pipe_task_manager_->AddConnectTask(); }

				pipe_task_manager_->StartTasks();
				pipe_task_manager_->WaitOnTasks();
				pipe_task_manager_->RemoveStrayTasks();
			break;
			case PIPE_CONNECTED :
				//allow only one read task at a time
				if( !pipe_task_manager_->IsTaskInList(TASK_TYPE_READ) ){ pipe_task_manager_->AddReadTask( pipe_params_.in_buffer_size, cv_thread_ ); }
				//if our cv_to_pipe queue has messages for us to write, do so
				CheckWriteQueue();

				pipe_task_manager_->StartTasks();
				pipe_task_manager_->WaitOnTasks();
				pipe_task_manager_->RemoveStrayTasks();
			break;
			case PIPE_BROKEN :
				if( pipe_handle_ != NULL ){
					CloseHandle(pipe_handle_);
					pipe_handle_ = NULL;
					blog( LOG_INFO, "[Lazysplits][%s] Pipe handle closed (pipe broken)" , thread_name_.c_str());
				}

				CreatePipe();
			break;
		}
		
		//inform CV thread if pipe just connected/disconnected
		if( pipe_state_ != state_last_loop){
			//connected
			if( pipe_state_ == PIPE_CONNECTED ){
				cv_thread_->MsgPipeConnected(true);
			}
			//disconnected
			else if( state_last_loop == PIPE_CONNECTED ){
				cv_thread_->MsgPipeConnected(false);
			}
		}
		state_last_loop = pipe_state_;
	}

	ThreadFuncCleanup();
	return NULL;
}

void LzsPipeServer::ThreadFuncCleanup(){
	if( pipe_handle_ != NULL ){
		CloseHandle(pipe_handle_);
		pipe_handle_ = NULL;
		blog( LOG_INFO, "[Lazysplits][%s] Pipe handle closed (thread terminating)" , thread_name_.c_str());
	}
	pipe_state_ = PIPE_NOT_CREATED;
	
	msg_queue_.DetachObserver(this);

	LzsThread::ThreadFuncCleanup();
}

void LzsPipeServer::ThreadTerminate(){
	if( pipe_state_ >= PIPE_CREATED ){
		if( pipe_task_manager_->IsWaiting() ){ pipe_task_manager_->CancelTasks(); }
	}

	LzsThread::ThreadTerminate();
}

bool LzsPipeServer::IsConnected(){
	return pipe_state_ == PIPE_CONNECTED;
}

void LzsPipeServer::MsgProtobuf( std::string serialized_protobuf ){
	msg_queue_.Push(serialized_protobuf);
	//pipe_task_manager_->AddWriteTask(serialized_protobuf);
}

void LzsPipeServer::OnSubjectNotify( const std::string& subject_name, const std::string& subject_message ){
	if( pipe_state_ >= PIPE_CREATED ){
		if( pipe_task_manager_->IsWaiting() ){ pipe_task_manager_->CancelWait(); }
	}
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
	while( !msg_queue_.IsEmpty() ){
		pipe_task_manager_->AddWriteTask( msg_queue_.Peek() );
		msg_queue_.Pop();
	}
}

} //namespace Lazysplits