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
		void virtual SetDefault( obs_data_t* source_settings ) = 0;
		void virtual UpdateProperty( obs_data_t* source_settings ) = 0;
	protected :
		std::string prop_name_;
		std::string prop_desc_;
};

class LzsObsPropBool : public LzsObsPropBase{
	public :
		LzsObsPropBool( bool* bool_ptr, std::string prop_name, std::string prop_desc );
		void AddProperty( obs_properties_t* source_properties )override;
		void SetDefault( obs_data_t* source_settings )override;
		void UpdateProperty( obs_data_t* source_settings )override;
	private :
		bool* bool_ptr_;
};

class LzsObsPropInt : public LzsObsPropBase{
	public :
		LzsObsPropInt( int64_t* source_int, std::string prop_name, std::string prop_desc, int int_min, int int_max, int int_step, bool use_slider );
		void AddProperty( obs_properties_t* source_properties )override;
		void SetDefault( obs_data_t* source_settings )override;
		void UpdateProperty( obs_data_t* source_settings )override;
	private :
		int64_t* int_ptr_;

		int int_min_;
		int int_max_;
		int int_step_;
		bool use_slider_;
};

class LzsObsPropFloat : public LzsObsPropBase{
	public :
		LzsObsPropFloat( double* float_ptr, std::string prop_name, std::string prop_desc, double float_min, double float_max, double float_step, bool use_slider );
		void AddProperty( obs_properties_t* source_properties )override;
		void SetDefault( obs_data_t* source_settings )override;
		void UpdateProperty( obs_data_t* source_settings )override;
	private :
		double* float_ptr_;

		double float_min_;
		double float_max_;
		double float_step_;
		bool use_slider_;
};

class LzsObsPropPath : public LzsObsPropBase{
	public :
		LzsObsPropPath( std::string* string_ptr_, std::string prop_name, std::string prop_desc, obs_path_type path_type, std::string filter_string, std::string default_path );
		void AddProperty( obs_properties_t* source_properties )override;
		void SetDefault( obs_data_t* source_settings )override;
		void UpdateProperty( obs_data_t* source_settings )override;
	private :
		std::string* string_ptr_;
		
		obs_path_type path_type_;
		std::string filter_string_;
		std::string default_path_;
};

class LzsObsPropList{
	public :
		void AddBool( bool* bool_ptr, std::string prop_name, std::string prop_desc );
		void AddInt( int64_t* int_ptr, std::string prop_name, std::string prop_desc, int int_min, int int_max, int int_step, bool use_slider = true );
		void AddFloat( double* float_ptr, std::string prop_name, std::string prop_desc, double int_min, double int_max, double int_step, bool use_slider = true );
		void AddPath( std::string* string_ptr, std::string prop_name, std::string prop_desc, obs_path_type path_type, std::string filter_string, std::string default_path );
		
		void AddProperties( obs_properties_t* source_properties );
		void SetPropertyDefaults( obs_data_t* source_settings );
		void UpdateProperties( obs_data_t* source_settings );
	private :
		std::vector<std::shared_ptr<LzsObsPropBase>> prop_list_;
};

} //namespace Lazysplits

