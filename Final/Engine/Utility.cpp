#include "Utility.hpp"

#include <SDL2/SDL.h>

#include <iostream>
#include <fstream>
#include <sstream>

namespace utility
{
	PathResolver::PathResolver()
	{
		AddPath(apathy::Path::cwd());
	}

	void PathResolver::AddPath(std::string path)
	{
		auto parsedPath = apathy::Path(path);
		if(parsedPath.is_directory())
			AddPath(parsedPath.directory());
		else
			AddPath(parsedPath.parent().directory());
	}

	void PathResolver::AddPath(apathy::Path path)
	{
		normalizedPaths.push_back(path.absolute());
	}

	std::string PathResolver::FindFile(std::string filename)
	{
		// blender starts relative paths with // for some reason...
		if(filename.size() > 2 && filename.substr(0, 2) == "//")
		{
			filename = filename.substr(2);
		}

		auto file = apathy::Path(filename);

		// we might not need to do anything, especially if it's an absolute path
		// so don't even bother
		if(file.exists()) 
			return filename;

		for(auto path : normalizedPaths)
		{
			auto joinedPath = path.relative(file);
			if(joinedPath.exists())
				return joinedPath.string();
		}

		return "";
	}

	std::string from_file(const std::string& filename)
	{
		std::ifstream in{ filename, std::ios::in | std::ios::binary };
		if(in)
		{
			std::ostringstream contents;
			contents << in.rdbuf();
			return contents.str();
		}
		throw (errno);
	}
}