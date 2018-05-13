#include "LzsSourceData.h"

#include <obs-module.h>
#include <obs-source.h>

#include <string>


/* TODO : limit to one filter instance in OBS*/

namespace Lazysplits{

OBS_DECLARE_MODULE()

static const char* lzs_source_name(void* type_data) {
    return "Lazysplits";
}

static void* lzs_source_create( obs_data_t* settings, obs_source_t* context ){
	LzsSourceData* source_data = new LzsSourceData( context, obs_get_module_data_path( obs_current_module() ) );
	source_data->OnSourceCreate( settings, context );
	return source_data;
}

static void lzs_source_destroy(void* data) { delete static_cast<LzsSourceData*>(data); }

static void lzs_source_video_tick( void *data, float seconds ){
	LzsSourceData* source_data = static_cast<LzsSourceData*>(data);
	source_data->OnSourceTick(seconds);
}

static void lzs_source_render_video(void *data, gs_effect_t *effect){
	LzsSourceData* source_data = static_cast<LzsSourceData*>(data);
	
	source_data->OnSourceRenderVideo(effect);
}

static obs_properties_t* lzs_source_properties(void* data)
{
	LzsSourceData* source_data = static_cast<LzsSourceData*>(data);

	/*
	obs_properties_t *props = obs_properties_create();
	obs_properties_add_path( props, "test_path", "test path", OBS_PATH_DIRECTORY, NULL, NULL );
	return props;
	*/

	return source_data->GetSourceProps();
}

static void lzs_source_update(void *data, obs_data_t *settings)
{
	LzsSourceData* source_data = static_cast<LzsSourceData*>(data);
	source_data->OnSourceUpdate(settings);
}

static void lzs_source_save( void* data, obs_data_t* settings ){
	LzsSourceData* source_data = static_cast<LzsSourceData*>(data);
	source_data->OnSourceSave(settings);
}

static void lzs_source_load( void* data, obs_data_t* settings ){
	blog( LOG_DEBUG, "[Lazysplits] lzs_source_load");
}

static struct obs_source_info lzs_source_info = {
    /* ----------------------------------------------------------------- */
    /* Required implementation*/

    /* id                  */ "lazysplits",
    /* type                */ OBS_SOURCE_TYPE_FILTER,
    /* output_flags        */ OBS_SOURCE_VIDEO,// | OBS_SOURCE_DO_NOT_DUPLICATE,
    /* get_name            */ lzs_source_name,
    /* create              */ lzs_source_create,
    /* destroy             */ lzs_source_destroy,
    /* get_width           */ 0,
    /* get_height          */ 0,

    /* ----------------------------------------------------------------- */
    /* Optional implementation */

    /* get_defaults        */ 0,
    /* get_properties      */ lzs_source_properties,
    /* update              */ lzs_source_update,
    /* activate            */ 0,
    /* deactivate          */ 0,
    /* show                */ 0,
    /* hide                */ 0,
    /* video_tick          */ lzs_source_video_tick,
    /* video_render        */ lzs_source_render_video,
    /* filter_video        */ 0,
    /* filter_audio        */ 0,
    /* enum_active_sources */ 0,
    /* save                */ lzs_source_save,//0,
    /* load                */ lzs_source_load,//0,
    /* mouse_click         */ 0,
    /* mouse_move          */ 0,
    /* mouse_wheel         */ 0,
    /* focus               */ 0,
    /* key_click           */ 0,
    /* filter_remove       */ 0,
    /* type_data           */ 0,
    /* free_type_data      */ 0,
    /* audio_render        */ 0
};

MODULE_EXPORT bool obs_module_load(void) {
	obs_register_source(&lzs_source_info);

	return true;
}

} //namespace Lazysplits