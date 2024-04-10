#include "Material.hpp"
#include "../Assets/HeightmapConverter.hpp"
#include "../Assets/TextureManager.hpp"
#include "Shader.hpp"

namespace eg { namespace Graphics {
	// to render bump maps we'd be computing their normals in the shader anyways
	// better to do it ahead of time and just upload the normal map
	eg::Texture *convertBumpMap(aiMaterial* mat, utility::PathResolver* resolver)
	{
		aiString path;
		if(mat->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS)
		{
			std::string realPath = resolver->FindFile(path.C_Str());
			if(realPath.empty())
			{
				LOG_WARNING << "Couldn't find texture " << path.C_Str() << " for mesh.";
				return NULL;
			}

			if(TextureManager::Has(realPath))
				return TextureManager::Load(realPath);

			LOG_INFO << "Converting bump map " << path.C_Str() << " to normal.";

			// load heightmap as grayscale (SOIL_LOAD_L)
			int width, height, channels;
			unsigned char *heightmap = SOIL_load_image(realPath.c_str(), &width, &height, &channels, SOIL_LOAD_L);

			HeightmapConverter converter(width, height, heightmap);
			unsigned char *normalmap = new unsigned char[width * height * 3];
			converter.ComputeNormalMap(normalmap);
			SOIL_free_image_data(heightmap);

			eg::Texture *normal = new eg::Texture();
			normal->Activate();
			switch(width % 8)
			{
			case 0:
				glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
				break;
			case 4:
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
				break;
			case 2:
			case 6:
				glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
				break;
			default:
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				break;
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, normalmap);
			delete normalmap;
			normal->Deactivate();

			TextureManager::Store(realPath, normal);

			LOG_INFO << "Successfully converted.";
			return normal;
		}
		else
		{
			LOG_ERROR << "Couldn't get bump/heightmap texture from mesh.";
			return NULL;
		}
	}

	eg::Texture *GetFirstTextureOfType(aiMaterial *mat, aiTextureType type, utility::PathResolver *resolver)
	{
		if(mat->GetTextureCount(type) == 0)
			return NULL;
		if(mat->GetTextureCount(type) > 1)
		{
			LOG_WARNING << "Only one diffuse texture per material supported.";
		}

		aiString path;
		if(mat->GetTexture(type, 0, &path) == AI_SUCCESS)
		{
			std::string realPath = resolver->FindFile(path.C_Str());
			if(realPath.empty())
			{
				LOG_WARNING << "Couldn't find texture " << path.C_Str() << " for mesh.";
				return NULL;
			}

			return TextureManager::Load(realPath);
		}
		else
		{
			LOG_ERROR << "Couldn't get texture from mesh.";
			return NULL;
		}
	}

	Material::Material(eg::Program* program, aiMaterial* mat, utility::PathResolver *resolver) : program(program)
	{
		// texture cache / texture loader class
		diffuseTexture = GetFirstTextureOfType(mat, aiTextureType_DIFFUSE, resolver);
		normalTexture = GetFirstTextureOfType(mat, aiTextureType_NORMALS, resolver);
		specularTexture = GetFirstTextureOfType(mat, aiTextureType_SPECULAR, resolver);
		if(specularTexture == NULL)
			specularTexture = GetFirstTextureOfType(mat, aiTextureType_SHININESS, resolver);
		alphaTexture = GetFirstTextureOfType(mat, aiTextureType_OPACITY, resolver);
		if(alphaTexture == NULL)
			alphaTexture = GetFirstTextureOfType(mat, aiTextureType_UNKNOWN, resolver);

		// this material has a heightmap, we need to convert it to a normal map
		if(mat->GetTextureCount(aiTextureType_HEIGHT) > 0)
		{
			assert(normalTexture == NULL);
			normalTexture = convertBumpMap(mat, resolver);
		}

		aiColor3D color(0.0f, 0.0f, 0.0f);
		float opacity = 1.0f;
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		mat->Get(AI_MATKEY_OPACITY, opacity);

		diffuseColor = vec4(color.r, color.g, color.b, opacity);

		mat->Get(AI_MATKEY_SHININESS, shininess);
		mat->Get(AI_MATKEY_SHININESS_STRENGTH, specularScale);
	}

	Material::~Material()
	{
		delete diffuseTexture;
		delete normalTexture;
		delete specularTexture;
		delete alphaTexture;
	}

	void Material::SetupRender()
	{
		// activate each texture if it exists
		if(HasDiffuseTexture())
			GetDiffuseTexture()->Activate(0);
		if(HasNormalTexture())
			GetNormalTexture()->Activate(1);
		if(HasSpecularTexture())
			GetSpecularTexture()->Activate(2);
		if(HasAlphaTexture())
			GetAlphaTexture()->Activate(3);

		program->Uniform<GLint>("txDiffuse") = 0;
		program->Uniform<GLint>("txNormal") = 1;
		program->Uniform<GLint>("txSpecular") = 2;
		program->Uniform<GLint>("txAlpha") = 3;

		// inform the shader what textures actually exist
		program->Uniform<GLint>("txAvailable[0]") = HasDiffuseTexture();
		program->Uniform<GLint>("txAvailable[1]") = HasNormalTexture();
		program->Uniform<GLint>("txAvailable[2]") = HasSpecularTexture();
		program->Uniform<GLint>("txAvailable[3]") = HasAlphaTexture();

		program->Uniform<vec4>("color") = GetDiffuseColor();
		program->Uniform<float>("specularScale") = GetSpecularScale();
		program->Uniform<float>("shininess") = GetShininess();
		program->Uniform<float>("roughness") = GetRoughness();
		program->Uniform<float>("albedo") = GetAlbedo();
	}

	void Material::CleanupRender()
	{
		eg::Texture::Deactivate(0);
		eg::Texture::Deactivate(1);
		eg::Texture::Deactivate(2);
		eg::Texture::Deactivate(3);
	}
}; };