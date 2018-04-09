#include "LzsSourceData.h"

#include <util\circlebuf.h>

namespace Lazysplits{

LzsSourceData::LzsSourceData( obs_source_t* context )
	:context_(context),
	 frame_count_(0),
	 cv_to_pipe_queue_("pipe queue"),
	 pipe_to_cv_queue_("cv queue"),
	 frame_buffer_( context, 30 ),
	 pipe_server_( "lazysplits_pipe", 8192, &cv_to_pipe_queue_, &pipe_to_cv_queue_ ),
	 cv_thread_( &frame_buffer_, &cv_to_pipe_queue_, &pipe_to_cv_queue_ )
{
	pipe_server_.ThreadCreate();
	cv_thread_.ThreadCreate();
}

LzsSourceData::~LzsSourceData(){
	if( pipe_server_.IsThreadLive() ){
		pipe_server_.ThreadTerminate();
		pipe_server_.ThreadJoin();
	}
	if( cv_thread_.IsThreadLive() ){
		cv_thread_.ThreadTerminate();
		cv_thread_.ThreadJoin();
	}
}

void LzsSourceData::FrameTick(){ frame_count_++; }

long LzsSourceData::FrameCount(){ return frame_count_; }

} //namespace Lazysplits