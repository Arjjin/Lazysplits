#include "LzsObsProps.h"

namespace Lazysplits{

//abstract base property

LzsObsPropBase::LzsObsPropBase( std::string prop_name, std::string prop_desc ){
	prop_name_ = prop_name;
	prop_desc_ = prop_desc;
}

//bool property

LzsObsPropBool::LzsObsPropBool(  bool* bool_ptr, std::string prop_name, std::string prop_desc )
	:LzsObsPropBase( prop_name, prop_desc )
{
	bool_ptr_ =  bool_ptr;
}

void LzsObsPropBool::AddProperty( obs_properties_t* source_properties ){
	obs_properties_add_bool( source_properties, prop_name_.c_str(), prop_desc_.c_str() );
}

void  LzsObsPropBool::SetDefault( obs_data_t* source_settings ){
	obs_data_set_default_bool( source_settings, prop_name_.c_str(), *bool_ptr_ );
}

void  LzsObsPropBool::UpdateProperty( obs_data_t* source_settings ){
	*bool_ptr_ = obs_data_get_bool( source_settings, prop_name_.c_str() );
}

//int property

LzsObsPropInt::LzsObsPropInt( int64_t* int_ptr, std::string prop_name, std::string prop_desc, int int_min, int int_max, int int_step, bool use_slider )
	:LzsObsPropBase( prop_name, prop_desc )
{
	int_ptr_ = int_ptr;

	int_min_ = int_min;
	int_max_ = int_max;
	int_step_ = int_step;
	use_slider_ = use_slider;
}

void LzsObsPropInt::AddProperty( obs_properties_t* source_properties ){
	(use_slider_)
		? obs_properties_add_int_slider( source_properties, prop_name_.c_str(), prop_desc_.c_str(), int_min_, int_max_, int_step_ )
		: obs_properties_add_int( source_properties, prop_name_.c_str(), prop_desc_.c_str(), int_min_, int_max_, int_step_ );
}

void LzsObsPropInt::SetDefault( obs_data_t* source_settings ){
	obs_data_set_default_int( source_settings, prop_name_.c_str(), *int_ptr_ );
}

void  LzsObsPropInt::UpdateProperty( obs_data_t* source_settings ){
	*int_ptr_ = obs_data_get_int( source_settings, prop_name_.c_str() );
}

//float property

LzsObsPropFloat::LzsObsPropFloat( double* float_ptr, std::string prop_name, std::string prop_desc, double float_min, double float_max, double float_step, bool use_slider )
	:LzsObsPropBase( prop_name, prop_desc )
{
	float_ptr_ = float_ptr;

	float_min_ = float_min;
	float_max_ = float_max;
	float_step_ = float_step;
}

void LzsObsPropFloat::AddProperty( obs_properties_t* source_properties ){
	(use_slider_)
		? obs_properties_add_float_slider( source_properties, prop_name_.c_str(), prop_desc_.c_str(), float_min_, float_max_, float_step_ )
		: obs_properties_add_float( source_properties, prop_name_.c_str(), prop_desc_.c_str(), float_min_, float_max_, float_step_ );
}

void LzsObsPropFloat::SetDefault( obs_data_t* source_settings ){
	obs_data_set_default_double( source_settings, prop_name_.c_str(), *float_ptr_ );
}

void  LzsObsPropFloat::UpdateProperty( obs_data_t* source_settings ){
	*float_ptr_ = obs_data_get_double( source_settings, prop_name_.c_str() );
}

//file/folder path property

LzsObsPropPath::LzsObsPropPath( std::string* string_ptr, std::string prop_name, std::string prop_desc, obs_path_type path_type, std::string filter_string, std::string default_path )
	:LzsObsPropBase( prop_name, prop_desc )
{
	string_ptr_ = string_ptr;

	path_type_ = path_type;
	filter_string_ = filter_string;
	default_path_ = default_path;
}

void LzsObsPropPath::AddProperty( obs_properties_t* source_properties ){
	obs_properties_add_path( source_properties, prop_name_.c_str(), prop_desc_.c_str(), path_type_, filter_string_.c_str(), default_path_.c_str() );
}

void LzsObsPropPath::SetDefault( obs_data_t* source_settings ){
	obs_data_set_default_string( source_settings, prop_name_.c_str(), (*string_ptr_).c_str() );
}

void LzsObsPropPath::UpdateProperty(  obs_data_t* source_settings  ){
	//*string_ptr_ = obs_data_get_string( source_settings, prop_name_.c_str() );
	(*string_ptr_).assign( obs_data_get_string( source_settings, prop_name_.c_str() ) );
}

//property list

void LzsObsPropList::AddBool( bool* bool_ptr, std::string prop_name, std::string prop_desc ){
	prop_list_.push_back( std::make_shared<LzsObsPropBool>( bool_ptr, prop_name, prop_desc ) );
}

void LzsObsPropList::AddInt( int64_t* int_ptr, std::string prop_name, std::string prop_desc, int int_min, int int_max, int int_step, bool use_slider ){
	prop_list_.push_back(
		std::make_shared<LzsObsPropInt>(
			int_ptr,
			prop_name,
			prop_desc,
			int_min,
			int_max,
			int_step,
			use_slider
		)
	);
}

void LzsObsPropList::AddFloat( double* float_ptr, std::string prop_name, std::string prop_desc, double float_min, double float_max, double float_step, bool use_slider ){
	prop_list_.push_back(
		std::make_shared<LzsObsPropFloat>(
			float_ptr,
			prop_name,
			prop_desc,
			float_min,
			float_max,
			float_step,
			use_slider
		)
	);
}

void LzsObsPropList::AddPath( std::string* string_ptr, std::string prop_name, std::string prop_desc, obs_path_type path_type, std::string filter_string, std::string default_path ){
	prop_list_.push_back(
		std::make_shared<LzsObsPropPath>(
			string_ptr,
			prop_name,
			prop_desc,
			path_type,
			filter_string,
			default_path
		)
	);
}

void LzsObsPropList::AddProperties( obs_properties_t* source_properties ){
	for( auto props_it =prop_list_.begin(); props_it != prop_list_.end(); ++props_it ){
		props_it->get()->AddProperty(source_properties);
	}
}

void LzsObsPropList::SetPropertyDefaults( obs_data_t* source_settings ){
	for( auto props_it =prop_list_.begin(); props_it != prop_list_.end(); ++props_it ){
		(*props_it)->SetDefault(source_settings);
	}
}

void LzsObsPropList::UpdateProperties( obs_data_t* source_settings ){
	for( auto props_it =prop_list_.begin(); props_it != prop_list_.end(); ++props_it ){
		(*props_it)->UpdateProperty(source_settings);
	}
}

} //namespace Lazysplits
