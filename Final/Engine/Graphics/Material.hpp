#pragma once

#include "Texture.hpp"
#include "Shader.hpp"
#include "../Utility.hpp"
#include <assimp/material.h>

namespace eg { namespace Graphics {
	class Material
	{
	public:
		Material(eg::Program* program, aiMaterial* material, utility::PathResolver *resolver);
		~Material();

		void SetupRender();
		void CleanupRender();

		eg::Program* GetProgram() const { return program; }

		bool HasDiffuseTexture() { return diffuseTexture != NULL; }
		eg::Texture *GetDiffuseTexture() { return diffuseTexture; }

		bool HasNormalTexture() { return normalTexture != NULL; }
		eg::Texture *GetNormalTexture() { return normalTexture; }

		bool HasSpecularTexture() { return specularTexture != NULL; }
		eg::Texture *GetSpecularTexture() { return specularTexture; }

		bool HasAlphaTexture() { return alphaTexture != NULL; }
		eg::Texture *GetAlphaTexture() { return alphaTexture; }

		glm::vec4 GetDiffuseColor() const { return diffuseColor; }
		float GetSpecularScale() const { return specularScale; }
		float GetShininess() const { return shininess; }
		float GetAlbedo() const { return albedo; }
		float GetRoughness() const { return roughness; }
	private:
		eg::Program *program;
		eg::Texture *diffuseTexture = NULL;
		eg::Texture *normalTexture = NULL;
		eg::Texture *specularTexture = NULL;
		eg::Texture *alphaTexture = NULL;
		float specularScale = 0.65f, shininess = 20.0f, albedo = 0.95f, roughness = 1.0f;
		glm::vec4 diffuseColor;
	};
}; };