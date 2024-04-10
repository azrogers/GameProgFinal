//
//  Texture.cpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/16/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include "Texture.hpp"
#include <SOIL2.h>

namespace eg {
	template<>
	Name<Texture>::Name()
	{
		glGenTextures(1, &name);
	}

	template<>
	Name<Texture>::~Name()
	{
		if(name) glDeleteTextures(1, &name);
	}

	Texture::Texture() :
		Name()
	{}


	Texture::Texture(
		const char *posX, 
		const char *negX, 
		const char *posY, 
		const char *negY, 
		const char *posZ, 
		const char *negZ) : 
		textureType(GL_TEXTURE_CUBE_MAP),
		Name()
	{
		name = SOIL_load_OGL_cubemap(
			posX,
			negX,
			posY,
			negY,
			posZ,
			negZ,
			SOIL_LOAD_AUTO,
			name,
			SOIL_FLAG_MIPMAPS);

		if(name == 0)
		{
			LOG_ERROR << "SOIL loading error: " << SOIL_last_result();
		}

		if(glGetError() != GL_NO_ERROR)
		{
			LOG_ERROR << "OpenGL error when loading texture.";
		}

		Activate();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		if(GLEW_EXT_texture_filter_anisotropic)
		{
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
		}

		Deactivate();
	}

	Texture::Texture(const char *filename) : Name()
	{
		LOG_INFO << "Loading texture " << filename;
		name = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, name, SOIL_FLAG_INVERT_Y);
		if(name == 0)
		{
			LOG_ERROR << "SOIL loading error: " << SOIL_last_result();
		}

		Activate();
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if(GLEW_EXT_texture_filter_anisotropic)
		{
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
		}

		Deactivate();
	}

	GLint Texture::Activate(GLint index) const
	{
		glActiveTexture(index + GL_TEXTURE0);
		glBindTexture(textureType, name);
		return index;
	}

	GLint Texture::Deactivate(GLint index)
	{
		glActiveTexture(index + GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		return index;
	}
}