//
//  Texture.hpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/16/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once
#include <SOIL2.h>
#include <plog/Log.h>

#include "../OpenGL.hpp"

namespace eg {
	class Texture : public Name<Texture>
	{
	public:
		Texture();

		Texture(const char *filename);

		// Cubemap
		Texture(
			const char *posX,
			const char *negX,
			const char *posY,
			const char *negY,
			const char *posZ,
			const char *negZ
		);

		template<typename T>
		Texture(T const& source) : Name() { Load(source); }

		template <class T>
		Texture& Load(T const& source);

		GLint Activate(GLint index = 0) const;

		static GLint Deactivate(GLint index = 0);
	private:
		GLenum textureType = GL_TEXTURE_2D;
	};
};