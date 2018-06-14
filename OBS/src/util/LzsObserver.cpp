#include "LzsObserver.h"

namespace Lazysplits{

LzsObserver::LzsObserver(){
	id_ = ++id_ref_;
}

int LzsObserver::GetId(){
	return id_;
}
std::atomic_int LzsObserver::id_ref_ = -1;

LzsObservable::LzsObservable( const std::string& subject_name )
	:subject_name_(subject_name)
{
	pthread_mutex_init( &mutex_, NULL );
}

LzsObservable::~LzsObservable(){
	pthread_mutex_destroy(&mutex_);
}

void LzsObservable::AttachObserver( LzsObserver* observer ){
	LockMutex();
	observer_list_.push_back(observer);
	UnlockMutex();
}

void LzsObservable::DetachObserver( LzsObserver* observer ){
	LockMutex();
	auto observer_it = observer_list_.begin();
	for( auto observer_it = observer_list_.begin(); observer_it != observer_list_.end(); ++observer_it ){
		if( observer->GetId() == (*observer_it)->GetId() ){
			observer_list_.erase(observer_it);
			break;
		}
	}
	UnlockMutex();
}

void LzsObservable::NotifyAll( std::string message ){
	LockMutex();
	for( auto observer_it = observer_list_.begin(); observer_it != observer_list_.end(); ++observer_it ){
		(*observer_it)->OnSubjectNotify( subject_name_, message );
	}
	UnlockMutex();
}

void LzsObservable::LockMutex(){
	pthread_mutex_lock(&mutex_);
}

void LzsObservable::UnlockMutex(){
	pthread_mutex_unlock(&mutex_);
}

} //namespace Lazysplits