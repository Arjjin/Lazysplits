#include "LzsThread.h"

#include "obs.h"

namespace Lazysplits{

static void* run_thread( void* thread_ptr ){
	LzsThread* thread = (LzsThread*)thread_ptr;
	return thread->ThreadFunc();
}

LzsThread::LzsThread( std::string thread_name ){
	pthread_mutex_init( &thread_mutex_, NULL );
	pthread_cond_init( &COND_SHOULD_WAKE_, NULL );

	is_live_ = false;
	should_sleep_ = false;
	is_sleeping_ = false;
	should_wake_ = false;
	should_terminate_ = false;

	thread_name_ = thread_name;
}

LzsThread::~LzsThread(){
	is_live_ = false;
	pthread_mutex_destroy(&thread_mutex_);
	pthread_cond_destroy(&COND_SHOULD_WAKE_);
}

void LzsThread::ThreadFuncInit(){
	blog( LOG_DEBUG, "[Lazysplits][%s] ThreadFuncInit()", thread_name_.c_str() );

	/*
	not sure about leaving all of these disabled

	should_terminate in particular was fucking things up, parent threads cleanup functions were
	terminating and joining on child threads before they've been created

	if this happens the child thread overrides should_terminate to false in this init function
	when it DOES get created and never breaks out of the main loop
	*/

	//is_live_ = true;
	//should_sleep_ = false;
	//is_sleeping_ = false;
	//should_wake_ = false;
	//should_terminate_ = false;
}

bool LzsThread::ThreadFuncShouldLoop(){
	if( should_sleep_ ){ ThreadSleepInternal(); }
	if( should_terminate_ ){
		blog( LOG_DEBUG, "[Lazysplits][%s] ThreadFuncShouldLoop(): thread terminating", thread_name_.c_str() );
		return false;
	}

	return true;
}

void LzsThread::ThreadFuncCleanup(){
	blog( LOG_DEBUG, "[Lazysplits][%s] ThreadFuncCleanup()", thread_name_.c_str() );

	is_live_ = false;
	should_sleep_ = false;
	is_sleeping_ = false;
	should_wake_ = false;
	should_terminate_ = false;
}

void LzsThread::ThreadCreate(){
	if(is_live_){
		blog( LOG_DEBUG, "[Lazysplits][%s] ThreadCreate() : thread is already live!", thread_name_.c_str() );
		return;
	}

	int thread_error = pthread_create( &thread_id_, NULL, run_thread, this );

	if( thread_error == 0 ){
		is_live_ = true;
		blog( LOG_DEBUG, "[Lazysplits][%s] thread created", thread_name_.c_str() );
	}
	else{ blog( LOG_ERROR, "[Lazysplits][%s] thread creation failed!", thread_name_.c_str() ); }
}

void LzsThread::ThreadJoin(){
	if(is_live_){
		pthread_join( thread_id_, NULL );
		blog( LOG_DEBUG, "[Lazysplits][%s] joined", thread_name_.c_str() );
	}
	else{ blog( LOG_DEBUG, "[Lazysplits][%s] join attempted before thread has been created!", thread_name_.c_str() ); }
}

void LzsThread::ThreadSleep(){
	if( should_sleep_ ){ blog( LOG_DEBUG, "[Lazysplits][%s] thread is already signalled to sleep!", thread_name_.c_str() ); }
	else if( is_sleeping_ ){ blog( LOG_DEBUG, "[Lazysplits][%s] sleep function called while thread is already sleeping!", thread_name_.c_str() ); }
	else{
		should_sleep_ = true;
		blog( LOG_DEBUG, "[Lazysplits][%s] sleep signalled", thread_name_.c_str() );
	}
}

void LzsThread::ThreadWake(){
	if( is_sleeping_ ){
		blog( LOG_DEBUG, "[Lazysplits][%s] waking thread", thread_name_.c_str() );
		should_wake_ = true;

		pthread_cond_signal(&COND_SHOULD_WAKE_);
	}
	else{ blog( LOG_DEBUG, "[Lazysplits][%s] wake function called while thread is awake!", thread_name_.c_str() ); }
}

void LzsThread::ThreadTerminate(){
	if( is_live_ ){
		should_terminate_ = true;

		if(is_sleeping_){
			blog( LOG_DEBUG, "[Lazysplits][%s] attempting to wake thread for termination", thread_name_.c_str() );
			should_wake_ = true;
			ThreadWake();
		}
		
		blog( LOG_DEBUG, "[Lazysplits][%s] termination signalled", thread_name_.c_str() );
	}
}

void LzsThread::ThreadLockMutex(){ pthread_mutex_lock(&thread_mutex_); }

void LzsThread::ThreadUnlockMutex(){ pthread_mutex_unlock(&thread_mutex_); }

bool LzsThread::IsThreadLive(){ return is_live_; }

bool LzsThread::IsThreadSleeping(){ return is_sleeping_; }

void LzsThread::ThreadSleepInternal(){
	if(is_sleeping_){ return; }

	pthread_mutex_lock(&thread_mutex_);
	
	is_sleeping_ = true;
	should_sleep_ = false;
	
	blog( LOG_DEBUG, "[Lazysplits][%s] thread sleeping", thread_name_.c_str() );
	while(!should_wake_){
		pthread_cond_wait( &COND_SHOULD_WAKE_, &thread_mutex_ );
	}

	pthread_mutex_unlock(&thread_mutex_);
	
	blog( LOG_DEBUG, "[Lazysplits][%s] thread woke", thread_name_.c_str() );

	is_sleeping_ = false;
	should_wake_ = false;
}

}//namespace Lazysplits