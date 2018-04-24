#include "LzsSharedData.h"

#include <filesystem>

namespace filesys = std::experimental::filesystem;

namespace Lazysplits{
namespace SharedData{

bool PathHasGameList( std::string path ){
	filesys::path root_dir = path;
	if( filesys::is_regular_file( root_dir /= "GameList.json" ) ){ return true; }

	return false;
}

} //namespace SharedData
} //namespace Lazysplits