#pragma once

#include <string>
#include <map>

#include <assimp/material.h>
#include <assimp/Logger.hpp>

#include "../Utility.hpp"
#include "../ECS/Object.hpp"

namespace eg
{
	class AssimpLoader
	{
	public:
		static eg::Object *Load(std::string filename, eg::Program *program);
	private:
		static Assimp::Logger* assimpLogger;
	};
}