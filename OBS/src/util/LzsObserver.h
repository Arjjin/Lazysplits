#pragma once

#include <vector>
#include <string>

namespace Lazysplits{

//forward declaration to prevent circular refrence
class LzsObserver;

class LzsObservable{
	public :
		LzsObservable( const std::string& subject_name );
		void AttachObserver( LzsObserver* observer );
		void DetachObserver( LzsObserver* observer );
		void NotifyAll( std::string message = "" );
	private :
		const std::string subject_name_;
		std::vector<LzsObserver*> observer_list_;
};

class LzsObserver{
	public :
		~LzsObserver();
		virtual void OnSubjectNotify( const std::string& subject_name, const std::string& subject_message ) = 0;
		void Attached( LzsObservable* subject );
		void Detached( LzsObservable* subject );
	private :
		std::vector<LzsObservable*> subject_list_;
};

} //namespace Lazysplits