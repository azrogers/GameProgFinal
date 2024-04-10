//
//  World.hpp
//  GLProject1
//
//  Created by Nevin Flanagan on 1/17/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#include <string>
#include <vector>
using namespace std;

#include <assimp/mesh.h>

#include "../OpenGL.hpp"
#include "Buffer.hpp"
#include "Vertex.hpp"

namespace eg
{
	class Mesh
	{
	public:
		Mesh(aiMesh* mesh);
		Mesh(std::vector<Vertex> vertices, std::vector<GLushort> indices);
		std::string GetName() { return name; }
		unsigned int GetMaterialIndex() { return materialIndex; }

		void Render(void) const;
	protected:
		eg::Vertex::Array data;
		int faceCount;
		std::string name;
		unsigned int materialIndex;
	};
}
