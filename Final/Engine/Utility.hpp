#pragma once

#include <apathy.hpp>

namespace utility
{
	/**
	 * Resolves relative paths by looking in the given directories.
	 * Used to resolve texture names in the Assimp loader.
	 */
	class PathResolver
	{
	public:
		/**
		 * Create a new PathResolver with the current working directory
		 * as the first path.
		 */
		PathResolver();

		/**
		 * Adds the given string as a path.
		 * If given a filename, it'll use the directory of that file.
		 */
		void AddPath(std::string path);

		/**
		 * Same as above, but with apathy Paths (the path library we use)
		 */
		void AddPath(apathy::Path path);

		/**
		 * Look for the given path.
		 */
		std::string FindFile(std::string filename);
	private:
		std::vector<apathy::Path> normalizedPaths;
	};

	/**
	 * Reads a file into a string.
	 */
	std::string from_file(const std::string& filename);

	static std::string path_name(std::string& filename)
	{
		apathy::Path path(filename);
		return path.filename();
	}
}