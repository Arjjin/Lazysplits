#include "LzsSourceData.h"
#include <util\circlebuf.h>

namespace Lazysplits{

LzsSourceData::LzsSourceData( obs_source_t* context )
	:context_(context),
	 cv_to_pipe_queue_("pipe queue"),
	 pipe_to_cv_queue_("cv queue"),
	 frame_buffer_(30),
	 test_int(870),
	 pipe_server_( "lazysplits_pipe", 8192, &cv_to_pipe_queue_, &pipe_to_cv_queue_ ),
	 cv_thread_( &frame_buffer_, &cv_to_pipe_queue_, &pipe_to_cv_queue_ )
{
	frame_count_ = 0;
	shared_dir_root_ = "";
	
	test_bool = true;
	test_int = 870;

	obs_data_t* settings =  obs_source_get_settings(context);
	obs_data_set_default_bool( settings, "test_bool", test_bool );
	properties_.AddBool( &test_bool, "test_bool", "A test bool" );
	obs_data_set_default_int( settings, "test_int", test_int );
	properties_.AddInt( &test_int, "test_int", "A test int slider", 0, 1000, 10 );

	/*
	attach cv thread as observer to pipe thread so it can get informed of connection status of pipe while sleeping
	pretty ugly; bypasses what should be the communication bridge between the two (the message queues)
	prevents serializing/deserializing needlessly and awkwardly constructing messages inside pipe that aren't actually IPC
	*/
	pipe_server_.AttachObserver(&cv_thread_);

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

//void LzsSourceData::SourceUpdate( obs_data_t *settings ){
	/*
	std::string updated_shared_dir_root = obs_data_get_string( settings, SETTING_SHARED_DATA_PATH );
	if( new_shared_dir_root != source_data->shared_dir_root_ ){
		source_data->shared_dir_root_ = new_shared_dir_root;
		blog( LOG_DEBUG, "[Lazysplits] new shared data directory root");
	}
	*/
//}

void LzsSourceData::FrameTick(){ frame_count_++; }

long LzsSourceData::FrameCount(){ return frame_count_; }

} //namespace Lazysplits