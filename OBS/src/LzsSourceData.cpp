#include "LzsSourceData.h"

#include "SharedData\LzsSharedData.h"

namespace Lazysplits{

LzsSourceData::LzsSourceData( obs_source_t* context )
	:context_(context),
	 frame_buffer_(30),
	 pipe_server_( "lazysplits_pipe", 8192 ),
	 cv_thread_( &frame_buffer_ )
{
	pipe_server_.AssignCvThread(&cv_thread_);
	cv_thread_.AssignPipe(&pipe_server_);
	frame_count_ = 0;

	InitProps(context);

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

void LzsSourceData::InitProps( obs_source_t* context ){
	properties_.AddPath( &prop_shared_data_dir, "shared_data_path", "Shared data directory", OBS_PATH_DIRECTORY, "", "" );
	
	obs_data_t* source_settings = obs_source_get_settings(context);
	properties_.SetPropertyDefaults(source_settings);
	obs_data_release(source_settings);
}

void LzsSourceData::OnSourceCreate( obs_data_t* source_settings, obs_source_t* context ){
	properties_.UpdateProperties(source_settings);
	cv_thread_.MsgSetSharedDataPath(prop_shared_data_dir);
}

obs_properties_t* LzsSourceData::GetSourceProps(){
	obs_properties_t *props = obs_properties_create();
	properties_.AddProperties(props);

	return props;

}

void LzsSourceData::OnSourceUpdate( obs_data_t *settings ){
	std::string cur_shared_data_dir = prop_shared_data_dir;

	properties_.UpdateProperties(settings);

	if( prop_shared_data_dir != cur_shared_data_dir  ){
		cv_thread_.MsgSetSharedDataPath(prop_shared_data_dir);
	}
}

void LzsSourceData::OnSourceTick( float seconds ){ frame_count_++; }

void LzsSourceData::OnSourceFilterVideo( obs_source_frame* frame ){

}

long LzsSourceData::GetFrameCount(){ return frame_count_; }

} //namespace Lazysplits