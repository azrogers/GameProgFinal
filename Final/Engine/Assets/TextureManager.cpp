#include "TextureManager.hpp"

namespace eg
{
	std::unordered_map<std::string, eg::Texture *> TextureManager::textureCache = 
		std::unordered_map<std::string, eg::Texture*>();

	eg::Texture* TextureManager::Load(std::string pathStr)
	{
		std::string path = standardizePath(pathStr);
		if(textureCache.count(path) > 0)
			return textureCache[path];
		return textureCache[path] = new eg::Texture(path.c_str());
	}

	void TextureManager::Store(std::string pathStr, eg::Texture* texture)
	{
		textureCache[standardizePath(pathStr)] = texture;
	}

	bool TextureManager::Has(std::string pathStr)
	{
		return textureCache.count(standardizePath(pathStr)) > 0;
	}

	std::string TextureManager::standardizePath(std::string pathStr)
	{
		apathy::Path pathObj(pathStr);
		return pathObj.sanitize().string();
	}
};