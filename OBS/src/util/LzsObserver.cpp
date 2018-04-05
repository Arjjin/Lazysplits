#include "LzsObserver.h"

namespace Lazysplits{

LzsObserver::LzsObserver(){
	is_observer_attached_ = false;
}

LzsObserver::~LzsObserver(){
	//attempted thread safety, observer will detach itself from subject when destructor is called
	if(is_observer_attached_){ observer_subject_->DetachObserver(this); }
}

void LzsObserver::Attached( LzsObservable* subject ){
	//bad stuff with multiple subjects
	observer_subject_ = subject;
	is_observer_attached_ = true;
}

void LzsObserver::Detached(){
	is_observer_attached_ = false;
	observer_subject_ = nullptr;
}

void LzsObservable::AttachObserver( LzsObserver* observer ){
	observer_list_.push_back(observer);
	observer->Attached(this);
}

void LzsObservable::DetachObserver( LzsObserver* observer ){
	for( auto observer_it = observer_list_.begin(); observer_it != observer_list_.end(); ++observer_it ){
		//comparing pointers for identity, good enough?
		if( observer == *observer_it ){
			observer->Detached();
			observer_list_.erase(observer_it);
			return;
		}
	}
}

void LzsObservable::NotifyAll(){
	for( auto observer_it = observer_list_.begin(); observer_it != observer_list_.end(); ++observer_it ){
		(*observer_it)->OnSubjectNotify();
	}
}

} //namespace Lazysplits