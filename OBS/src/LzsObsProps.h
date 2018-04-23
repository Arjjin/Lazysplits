#pragma once

#include <string>
#include <vector>
#include <memory>

#include <obs-module.h>


namespace Lazysplits{

class LzsObsPropBase{
	public :
		LzsObsPropBase( std::string prop_name, std::string prop_desc );

		void virtual AddProperty( obs_properties_t* source_properties ) = 0;
		void virtual UpdateProperty( obs_data_t* source_settings ) = 0;
	protected :
		std::string prop_name_;
		std::string prop_desc_;
};

class LzsObsPropBool : public LzsObsPropBase{
	public :
		LzsObsPropBool( bool* source_bool, std::string prop_name, std::string prop_desc );
		void AddProperty( obs_properties_t* source_properties )override;
		void UpdateProperty( obs_data_t* source_settings )override;
	private :
		bool* source_bool_;
};

class LzsObsPropInt : public LzsObsPropBase{
	public :
		LzsObsPropInt( int64_t* source_int, std::string prop_name, std::string prop_desc, int int_min, int int_max, int int_step, bool use_slider );
		void AddProperty( obs_properties_t* source_properties )override;
		void UpdateProperty( obs_data_t* source_settings )override;
	private :
		int64_t* source_int_;

		int int_min_;
		int int_max_;
		int int_step_;
		bool use_slider_;
};

class LzsObsPropList{
	public :
		void AddBool( bool* source_bool, std::string prop_name, std::string prop_desc );
		void AddInt( int64_t* source_int, std::string prop_name, std::string prop_desc, int int_min, int int_max, int int_step, bool use_slider = true );

		void AddProperties( obs_properties_t* source_properties );
		void UpdateProperties( obs_data_t* source_settings );
	private :
		std::vector<std::shared_ptr<LzsObsPropBase>> prop_list_;
};

} //namespace Lazysplits

