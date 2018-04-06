#include "LzsSourceData.h"

namespace Lazysplits{

LzsSourceData::LzsSourceData( obs_source_t* context )
	:context_(context),pipe_server_( "lazysplits_pipe", 8192, &cv_to_pipe_queue_, &pipe_to_cv_queue_ )
{
	pipe_server_.ThreadCreate();
}

LzsSourceData::~LzsSourceData(){
	if( pipe_server_.IsThreadLive() ){
		pipe_server_.ThreadTerminate();
		pipe_server_.ThreadJoin();
	}
}

} //namespace Lazysplits