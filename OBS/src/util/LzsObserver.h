#pragma once

#include <vector>

namespace Lazysplits{

//forward declaration to prevent circular refrence between LzsObserver and LzsSubject
class LzsObserver;

class LzsObservable{
	public :
		void AttachObserver( LzsObserver* observer );
		void DetachObserver( LzsObserver* observer );
		void NotifyAll();
	private :
		std::vector<LzsObserver*> observer_list_;
};

class LzsObserver{
	public :
		LzsObserver();
		~LzsObserver();
		virtual void OnSubjectNotify() = 0;
		void Attached( LzsObservable* subject );
		void Detached();
	private :
		bool is_observer_attached_;
		LzsObservable* observer_subject_;
};

} //namespace Lazysplits