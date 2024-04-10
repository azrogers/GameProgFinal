#pragma once

#include <unordered_map>
#include <apathy.hpp>
#include <string>
#include "../Graphics/Texture.hpp"

namespace eg
{
	class TextureManager
	{
	public:
		static eg::Texture *Load(std::string pathStr);

		static void Store(std::string pathStr, eg::Texture* texture);
		static bool Has(std::string pathStr);
	private:
		static std::string standardizePath(std::string pathStr);
		static std::unordered_map<std::string, eg::Texture*> textureCache;
	};
};