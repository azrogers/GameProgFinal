//
//  OpenGL.h
//  GLhomework1
//
//  Created by Nevin Flanagan on 1/29/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <string>
#include <cstddef>
#include <json.hpp>

#define VEC3_TO_DDVEC3(v3) ddVec3 {v3.x, v3.y, v3.z}
#define MAT4_TO_DDMAT4X4(m4) ddMat4x4 { \
	m4[0][0], m4[1][0], m4[2][0], m4[3][0],\
	m4[0][1], m4[1][1], m4[2][1], m4[3][1],\
	m4[0][2], m4[1][2], m4[2][2], m4[3][2],\
	m4[0][3], m4[1][3], m4[2][3], m4[3][3] }

#include "glm/glm.hpp"
using glm::vec2; using glm::vec3; using glm::vec4; using glm::mat4;

namespace eg {
    template <class T>
    class Name {
    public:
        Name(Name && source) { name = source.name; source.name = 0; }
        operator bool() { return name != 0; }
        Name& operator= (Name && source) { name = source.name; source.name = 0; return *this; }
    protected:
        Name();
        Name(GLuint existing): name(existing) {}
        ~Name();
        
        GLuint name;
    };
    
	typedef vec3 Point3;
    typedef vec3 Vector3;
    typedef vec2 Vector2;
}

namespace glm
{
	static void to_json(nlohmann::json& j, const vec3& vec)
	{
		j = { { "x", vec.x },{ "y", vec.y },{ "z", vec.z } };
	}

	static void to_json(nlohmann::json& j, const vec2& vec)
	{
		j = { { "x", vec.x },{ "y", vec.y } };
	}
};

namespace shader {
    extern std::string vFlat;
    extern std::string fFlat;
    extern std::string vPhong;
    extern std::string fPhong;
}