#include "LzsObserver.h"

#include <obs.h>

namespace Lazysplits{

//TODO : (1) all the nested vector looping can be extremely bad with many subjects/observers connected to each other, hard to tell if iterators will remain valid (2) nothing has any thread safety

LzsObservable::LzsObservable( const std::string& subject_name )
	:subject_name_(subject_name)
{}

void LzsObservable::AttachObserver( LzsObserver* observer ){
	observer_list_.push_back(observer);
	observer->Attached(this);
}

void LzsObservable::DetachObserver( LzsObserver* observer ){
	for( auto observer_it = observer_list_.begin(); observer_it != observer_list_.end(); ++observer_it ){
		//comparing pointers for identity, good enough?
		if( observer == *observer_it ){
			observer->Detached(this);
			observer_list_.erase(observer_it);
			return;
		}
	}
}

void LzsObservable::NotifyAll( std::string message ){
	for( auto observer_it = observer_list_.begin(); observer_it != observer_list_.end(); ++observer_it ){
		(*observer_it)->OnSubjectNotify( subject_name_, message );
	}
}

LzsObserver::~LzsObserver(){
	//observer will detach itself from subject/subjects when destructor is called
	while( !subject_list_.empty() ){
		subject_list_.front()->DetachObserver(this);
	}
}

void LzsObserver::Attached( LzsObservable* subject ){ subject_list_.push_back(subject); }

void LzsObserver::Detached( LzsObservable* subject ){
	for( auto subject_it = subject_list_.begin(); subject_it != subject_list_.end(); ++subject_it ){
		if( (*subject_it) == subject ){
			subject_list_.erase(subject_it);
			return;
		}
	}
}

} //namespace Lazysplits