#include <obs-module.h>

#include "LzsSourceData.h"

#include <string>

/* TODO : limit to one filter instance in OBS*/

namespace Lazysplits{

static const char* lzs_source_name(void* type_data) {
    return "Lazysplits";
}

static void lzs_source_update(void *data, obs_data_t *settings){

}

static void* lzs_source_create( obs_data_t* settings, obs_source_t* context ){
	return static_cast<void*>( new LzsSourceData(context) );
}

static void lzs_source_destroy(void* data) {
	delete static_cast<LzsSourceData*>(data);
}

static void lzs_source_video_tick( void *data, float seconds ){
	LzsSourceData* source_data = static_cast<LzsSourceData*>(data);
	source_data->FrameTick();
}

static void lzs_source_render_video(void *data, gs_effect_t *effect){

}

static struct obs_source_frame* lzs_source_filter_video( void* data, struct obs_source_frame* frame){
	LzsSourceData* source_data = static_cast<LzsSourceData*>(data);

	if( source_data->cv_thread_.IsCvActive() ){ source_data->frame_buffer_.PushFrame(frame); }

	return frame;
}

/*
static bool offset_calibration_toggle( obs_properties_t *props, obs_property_t *p, obs_data_t *settings ){
	bool calib_toggle = obs_data_get_bool( settings, SETTING_IS_OFFSET_CALIBRATION );
	
	obs_property_set_visible( obs_properties_get( props, SETTING_CALIB_IMAGE_PATH ), calib_toggle );
	obs_property_set_visible( obs_properties_get( props, SETTING_CALIB_IMAGE_OPACITY ), calib_toggle );
	obs_property_set_visible( obs_properties_get( props, SETTING_OFFSET_X ), calib_toggle );
	obs_property_set_visible( obs_properties_get( props, SETTING_OFFSET_Y ), calib_toggle );
	obs_property_set_visible( obs_properties_get( props, SETTING_SCALE_X ), calib_toggle );
	obs_property_set_visible( obs_properties_get( props, SETTING_SCALE_Y ), calib_toggle );

	return true;
}
*/

/*
static obs_properties_t* lzs_filter_properties(void* data)
{
	obs_properties_t *props = obs_properties_create();
	
	obs_properties_add_int_slider( props, SETTING_POLLING_MS, TEXT_POLLING_MS, 5, 1000, 5 );
	obs_properties_add_int_slider( props, SETTING_CV_FRAME_SKIP, TEXT_CV_FRAME_SKIP, 1, 30, 1 );
	
	obs_property_t *offset_calibration = obs_properties_add_bool( props, SETTING_IS_OFFSET_CALIBRATION, TEXT_IS_OFFSET_CALIBRATION );
	obs_property_set_modified_callback( offset_calibration, offset_calibration_toggle );
	
	//TODO : narrow this down
	std::string calib_img_path = obs_module_file("img/calibration");
	std::string calib_img_filter_str =  "All Image Files (*.bmp *.jpg *.jpeg *.tga *.gif *.png);;All Files (*.*)";


	obs_properties_add_path( props, SETTING_CALIB_IMAGE_PATH, TEXT_CALIB_IMAGE_PATH, OBS_PATH_FILE, calib_img_filter_str.c_str(), calib_img_path.c_str() );

	obs_properties_add_float_slider( props, SETTING_CALIB_IMAGE_OPACITY, TEXT_CALIB_IMAGE_OPACITY, 0.0, 100.0, 1.00 );
	obs_properties_add_float_slider( props, SETTING_OFFSET_X, TEXT_OFFSET_X, -50.0, 50.0, 0.05 );
	obs_properties_add_float_slider( props, SETTING_OFFSET_Y, TEXT_OFFSET_Y, -50.0, 50.0, 0.05 );
	obs_properties_add_float_slider( props, SETTING_SCALE_X, TEXT_SCALE_X, 20.0, 500.0, 0.05 );
	obs_properties_add_float_slider( props, SETTING_SCALE_Y, TEXT_SCALE_Y, 20.0, 500.0, 0.05 );

	return props;
}
*/

static struct obs_source_info lzs_source_info = {
    /* ----------------------------------------------------------------- */
    /* Required implementation*/

    /* id                  */ "lazysplits",
    /* type                */ OBS_SOURCE_TYPE_FILTER,
    /* output_flags        */ OBS_SOURCE_VIDEO,
    /* get_name            */ lzs_source_name,
    /* create              */ lzs_source_create,
    /* destroy             */ lzs_source_destroy,
    /* get_width           */ 0,
    /* get_height          */ 0,

    /* ----------------------------------------------------------------- */
    /* Optional implementation */

    /* get_defaults        */ 0,//lzs_filter_defaults,
    /* get_properties      */ 0,//lzs_filter_properties,
    /* update              */ 0,//lzs_filter_update,
    /* activate            */ 0,
    /* deactivate          */ 0,
    /* show                */ 0,
    /* hide                */ 0,
    /* video_tick          */ lzs_source_video_tick,
    /* video_render        */ 0,//lzs_filter_render_video,
    /* filter_video        */ lzs_source_filter_video,
    /* filter_audio        */ 0,
    /* enum_active_sources */ 0,
    /* save                */ 0,
    /* load                */ 0,
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

MODULE_EXPORT const char* obs_module_name(void) {
    return "SM64 Lazy Splits";
}

OBS_DECLARE_MODULE()

MODULE_EXPORT bool obs_module_load(void) {
	obs_register_source(&lzs_source_info);

	return true;
}

} //namespace Lazysplits