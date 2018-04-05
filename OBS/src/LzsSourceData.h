#pragma once

#include <obs-module.h>

namespace Lazysplits{

class LzsSourceData{
	public :
		LzsSourceData( obs_source_t* context );
	private :
		obs_source_t* context_;
};

} //namespace Lazysplits