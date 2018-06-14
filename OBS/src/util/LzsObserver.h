#pragma once

#include <vector>
#include <string>
#include <atomic>
#include <memory>

#include <util\threading.h>

namespace Lazysplits{

class LzsObserver{
	public :
		LzsObserver();
		virtual void OnSubjectNotify( const std::string& subject_name, const std::string& subject_message ) = 0;
		int GetId();
	private :
		static std::atomic_int id_ref_;
		int id_;
};

class LzsObservable{
	public :
		LzsObservable( const std::string& subject_name );
		~LzsObservable();
		void AttachObserver( LzsObserver* observer );
		void DetachObserver( LzsObserver* observer );
		void NotifyAll( std::string message = "" );
	private :
		void LockMutex();
		void UnlockMutex();

		pthread_mutex_t mutex_;
		const std::string subject_name_;
		std::vector<LzsObserver*> observer_list_;
};

} //namespace Lazysplits