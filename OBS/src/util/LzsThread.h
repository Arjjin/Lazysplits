#pragma once

#include <atomic>
#include <string>

#include <util\threading.h>

namespace Lazysplits{

class LzsThread {
	public :
		LzsThread( std::string thread_name );
		virtual ~LzsThread();

		//main thread function to override, pure virtual
		//ThreadFuncInit, ThreadFuncShouldLoop and ThreadFuncCleanup are used to construct the main body of this overriden function
		virtual void* ThreadFunc() = 0;

		//blank logging function to optionally override


		//create new thread running ThreadFunc();
		void ThreadCreate(); 
		void ThreadSleep();
		void ThreadWake();
		void ThreadJoin();
		virtual void ThreadTerminate();
		
		void ThreadLockMutex();
		void ThreadUnlockMutex();

		bool IsThreadLive();
		bool IsThreadSleeping();
	protected :
		std::atomic_bool is_live_;
		std::atomic_bool should_sleep_;
		std::atomic_bool is_sleeping_;
		std::atomic_bool should_wake_;
		std::atomic_bool should_terminate_;

		pthread_mutex_t thread_mutex_;
		//condition used to wake thread up if it has been put to sleep
		pthread_cond_t COND_SHOULD_WAKE_;

		std::string thread_name_;

		/*
		helper functions for overridden ThreadFunc, which should be constructed as such :

		* ThreadFuncInit first
		* ThreadFuncShouldLoop as the condition in a while loop that makes up the main function body.
		* ThreadFuncCleanup
		* return NULL
		*/
		virtual void ThreadFuncInit();
		virtual bool ThreadFuncShouldLoop();
		virtual void ThreadFuncCleanup();
		
		//internal sleep function, to be called from within ThreadFunc manually or by ThreadFuncShouldLoop
		void ThreadSleepInternal();
	private :
		pthread_t thread_id_;
};

} //namespace Lazysplits