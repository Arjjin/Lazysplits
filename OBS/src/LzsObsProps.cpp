#include "LzsObsProps.h"

namespace Lazysplits{

//abstract base property

LzsObsPropBase::LzsObsPropBase( std::string prop_name, std::string prop_desc ){
	prop_name_ = prop_name;
	prop_desc_ = prop_desc;
}

//bool property

LzsObsPropBool::LzsObsPropBool(  bool* source_bool, std::string prop_name, std::string prop_desc )
	:LzsObsPropBase( prop_name, prop_desc )
{
	source_bool_ = source_bool;
}

void LzsObsPropBool::AddProperty( obs_properties_t* source_properties ){
	obs_properties_add_bool( source_properties, prop_name_.c_str(), prop_desc_.c_str() );
}

void  LzsObsPropBool::UpdateProperty( obs_data_t* source_settings ){
	*source_bool_ = obs_data_get_bool( source_settings, prop_name_.c_str() );
}

//int property

LzsObsPropInt::LzsObsPropInt( int64_t* source_int, std::string prop_name, std::string prop_desc, int int_min, int int_max, int int_step, bool use_slider )
	:LzsObsPropBase( prop_name, prop_desc )
{
	source_int_ = source_int;
	int_min_ = int_min;
	int_max_ = int_max;
	int_step_ = int_step;
	use_slider_ = use_slider;
}

void LzsObsPropInt::AddProperty( obs_properties_t* source_properties ){
	//obs_source_get_settings( obs_source)
	(use_slider_)
		? obs_properties_add_int_slider( source_properties, prop_name_.c_str(), prop_desc_.c_str(), int_min_, int_max_, int_step_ )
		: obs_properties_add_int( source_properties, prop_name_.c_str(), prop_desc_.c_str(), int_min_, int_max_, int_step_ );
}

void  LzsObsPropInt::UpdateProperty( obs_data_t* source_settings ){
	*source_int_ = obs_data_get_int( source_settings, prop_name_.c_str() );
}

//property list

void LzsObsPropList::AddBool( bool* source_bool, std::string prop_name, std::string prop_desc ){
	prop_list_.push_back( std::make_shared<LzsObsPropBool>( source_bool, prop_name, prop_desc ) );
}

void LzsObsPropList::AddInt( int64_t* source_int, std::string prop_name, std::string prop_desc, int int_min, int int_max, int int_step, bool use_slider ){
	prop_list_.push_back(
		std::make_shared<LzsObsPropInt>(
			source_int,
			prop_name,
			prop_desc,
			int_min,
			int_max,
			int_step,
			use_slider
		)
	);
}

void LzsObsPropList::AddProperties( obs_properties_t* source_properties ){
	for( auto props_it =prop_list_.begin(); props_it != prop_list_.end(); ++props_it ){
		(*props_it)->AddProperty(source_properties);
	}
}

void LzsObsPropList::UpdateProperties( obs_data_t* source_settings ){
	for( auto props_it =prop_list_.begin(); props_it != prop_list_.end(); ++props_it ){
		(*props_it)->UpdateProperty(source_settings);
	}
}

} //namespace Lazysplits
