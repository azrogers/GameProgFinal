//
//  World.cpp
//  GLProject1
//
//  Created by Nevin Flanagan on 1/17/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "Mesh.hpp"

#include <json.hpp>

int loadShape(aiMesh* mesh, eg::Vertex::Array& vertexArray)
{
	vector<eg::Vertex> vertices;
	for(int i = 0; i < mesh->mNumVertices; i++)
	{
		eg::Vertex vertex;
		vertex.position = eg::Point3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z);

		// set normals if we have them, otherwise we'll compute them later
		if(mesh->mNormals != NULL)
			vertex.normal = eg::Point3(
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
			);

		if(mesh->HasTextureCoords(0))
		{
			vertex.uv = eg::Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.uv = eg::Vector2(0, 0);
		}

		vertices.push_back(vertex);
	}

	vector<GLushort> indices;
	for(int i = 0; i < mesh->mNumFaces; i++)
	{
		// NO QUADS ALLOWED
		assert(mesh->mFaces[i].mNumIndices == 3);

		for(int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}

		// if we don't have any precomputed normals, we can generate them here
		if(mesh->mNormals == NULL)
		{
			// following psuedo-code here: https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal#Pseudo-code
			eg::Vertex p1 = vertices.at(mesh->mFaces[i].mIndices[0]);
			eg::Vertex p2 = vertices.at(mesh->mFaces[i].mIndices[1]);
			eg::Vertex p3 = vertices.at(mesh->mFaces[i].mIndices[2]);
			glm::vec3 u = p2.position - p1.position;
			glm::vec3 v = p3.position - p1.position;
			glm::vec3 normal = glm::vec3(
				(u.y * v.z) - (u.z * v.y),
				(u.z * v.x) - (u.x * v.z),
				(u.x * v.y) - (u.y * v.x)
			);

			vertices.at(mesh->mFaces[i].mIndices[0]).normal = glm::vec3(normal);
			vertices.at(mesh->mFaces[i].mIndices[1]).normal = glm::vec3(normal);
			vertices.at(mesh->mFaces[i].mIndices[2]).normal = glm::vec3(normal);
		}
	}

	eg::Buffer vbo, ebo;
	vbo.Load(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices);
	vertexArray.Bind(vbo);

	vertexArray.Activate();
	ebo.Load(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices);
	vertexArray.Deactivate();

	vbo.Release();
	ebo.Release();

	return indices.size();
}

namespace eg
{
	Mesh::Mesh(aiMesh* mesh)
	{
		materialIndex = mesh->mMaterialIndex;
		name = std::string(mesh->mName.C_Str());
		faceCount = loadShape(mesh, data);
	}

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLushort> indices)
	{
		eg::Buffer vbo, ebo;
		vbo.Load(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices);
		data.Bind(vbo);

		data.Activate();
		ebo.Load(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices);
		data.Deactivate();

		vbo.Release();
		ebo.Release();

		faceCount = indices.size();
	}

	void Mesh::Render(void) const
	{
		data.Activate();
		GLushort position = 0;
		glDrawElements(GL_TRIANGLES, faceCount, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(position * sizeof(GLshort)));
		data.Deactivate();
	}
}
