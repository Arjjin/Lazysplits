#pragma once

#include <queue>

#include "obs.h"
#include <util\threading.h>

#include "..\util\LzsObserver.h"

namespace Lazysplits {

/* """thread safe""" message queue
simply duplicates queue methods with mutex locking/unlocking wrapped around them
race conditions for multiple threads between method calls still apply */
template <class T> class LzsMessageQueue : public LzsObservable{
public :
	LzsMessageQueue( std::string subject_name )
		:LzsObservable(subject_name)
	{ 
		pthread_mutex_init( &queue_mutex_, NULL );
	}
	~LzsMessageQueue(){ pthread_mutex_destroy(&queue_mutex_); }

	bool QueueIsEmpty(){
		pthread_mutex_lock(&queue_mutex_);
		bool is_empty = queue_.empty();
		pthread_mutex_unlock(&queue_mutex_);

		return is_empty;
	}
	const T& QueueFront(){
		pthread_mutex_lock(&queue_mutex_);
		const T& element = queue_.front();
		pthread_mutex_unlock(&queue_mutex_);

		return element;
	}
	void QueuePop(){
		pthread_mutex_lock(&queue_mutex_);
		queue_.pop();
		pthread_mutex_unlock(&queue_mutex_);
	}
	void QueuePush( const T& element ){
		pthread_mutex_lock(&queue_mutex_);
		queue_.push(element);
		pthread_mutex_unlock(&queue_mutex_);
		
		//notifies any observers subscribed to queue of element pushed in
		NotifyAll();
	}
private :
	std::queue<T> queue_;
	pthread_mutex_t queue_mutex_;
};

} //namespace Lazysplits