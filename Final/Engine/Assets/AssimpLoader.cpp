#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>

#include <plog/Log.h>
#include <SOIL2.h>

#include "AssimpLoader.hpp"
#include "../ECS/MeshRenderer.hpp"
#include "../Graphics/Material.hpp"
#include "../Utility.hpp"
#include "../Graphics/Light.hpp"

#define COLOR3D_TO_POINT3(c) eg::Point3(c.r, c.g, c.b)
#define VEC3D_TO_POINT3(c) eg::Point3(c.x, c.y, c.z)

class AssimpPlogLogger : public Assimp::Logger
{
public:
	AssimpPlogLogger() {}
	~AssimpPlogLogger() {}

	void OnDebug(const char* message) override
	{
		LOG_DEBUG << message;
	}

	void OnInfo(const char* message) override
	{
		LOG_INFO << message;
	}

	void OnWarn(const char* message) override
	{
		LOG_WARNING << message;
	}

	void OnError(const char* message) override
	{
		LOG_ERROR << message;
	}

	bool attachStream(Assimp::LogStream *pStream, unsigned int severity) override
	{
		return false;
	}

	bool detatchStream(Assimp::LogStream *pStream, unsigned int severity) override
	{
		return false;
	}
};

namespace eg
{
	Assimp::Logger* AssimpLoader::assimpLogger = NULL;

	eg::Object *createNode(
		eg::Object* parent, 
		aiNode* node, 
		vector<eg::Mesh *> meshes,
		vector<eg::Graphics::Material *> materials)
	{
		eg::Object* nodeObj = new eg::Object(parent);
		nodeObj->name = node->mName.C_Str();
		aiVector3D scaling, pos;
		aiQuaternion rot;
		node->mTransformation.Decompose(scaling, rot, pos);
		nodeObj->transform.position = VEC3D_TO_POINT3(pos);
		nodeObj->transform.scale = VEC3D_TO_POINT3(scaling);
		nodeObj->transform.rotation = glm::quat(
			rot.w,
			rot.x,
			rot.y,
			rot.z
		);

		for(int i = 0; i < node->mNumMeshes; i++)
		{
			eg::Mesh* mesh = meshes.at(node->mMeshes[i]);
			MeshRenderer* meshRenderer = new MeshRenderer(mesh, materials.at(mesh->GetMaterialIndex()));
			nodeObj->AddComponent(meshRenderer);
		}
		
		for(int i = 0; i < node->mNumChildren; i++)
		{
			createNode(nodeObj, node->mChildren[i], meshes, materials);
		}

		return nodeObj;
	}

	eg::Object *AssimpLoader::Load(std::string filename, eg::Program *program)
	{
		if(assimpLogger == nullptr)
		{
			assimpLogger = new AssimpPlogLogger();
			Assimp::DefaultLogger::set(assimpLogger);
		}

		LOG_INFO << "Loading model " << filename;
		Assimp::Importer importer;

		utility::PathResolver resolver;
		resolver.AddPath(filename);

		const aiScene* scene = importer.ReadFile(filename,
			aiProcess_Triangulate |
			aiProcess_SortByPType);

		if(scene == NULL)
		{
			throw runtime_error("Couldn't load assimp scene " + filename + ": " + importer.GetErrorString());
		}

		vector<eg::Mesh *> meshes;
		vector<eg::Graphics::Material *> materials;
		for(int i = 0; i < scene->mNumMeshes; i++)
		{
			eg::Mesh *mesh = new eg::Mesh(scene->mMeshes[i]);
			meshes.push_back(mesh);
		}

		for(int i = 0; i < scene->mNumMaterials; i++)
		{
			materials.push_back(new eg::Graphics::Material(program, scene->mMaterials[i], &resolver));
		}

		// build the hierarchy from assimp
		// this sometimes includes nodes that have no meshes, so we have do it seperately
		eg::Object *root = createNode(nullptr, scene->mRootNode, meshes, materials);

		/*if(scene->HasLights())
		{
			for(int i = 0; i < scene->mNumLights; i++)
			{
				aiLight* aLight = scene->mLights[i];
				eg::Light* light;
				switch(aLight->mType)
				{
					case aiLightSource_POINT:
						light = eg::Light::CreatePointLight(
							COLOR3D_TO_POINT3(aLight->mColorDiffuse),
							COLOR3D_TO_POINT3(aLight->mColorAmbient),
							aLight->mAttenuationLinear
						);
						break;
					case aiLightSource_SPOT:
						light = eg::Light::CreateSpotLight(
							COLOR3D_TO_POINT3(aLight->mColorDiffuse),
							COLOR3D_TO_POINT3(aLight->mColorAmbient),
							aLight->mAngleInnerCone / aLight->mAngleOuterCone,
							aLight->mAngleOuterCone
						);
						break;
					case aiLightSource_DIRECTIONAL:
						light = eg::Light::CreateDirLight(
							COLOR3D_TO_POINT3(aLight->mColorDiffuse),
							COLOR3D_TO_POINT3(aLight->mColorAmbient)
						);
						break;
					default:
						LOG_WARNING << "Unknown assimp light type: " << aLight->mType;
						continue;
				}

				aiNode* parentNode = scene->mRootNode->FindNode(aLight->mName);
				aiVector3D relativePos;
				aiQuaternion relativeRot;
				while(parentNode != NULL && parentNode->mNumMeshes == 0)
				{
					aiVector3D pos;
					aiQuaternion rot;
					parentNode->mTransformation.DecomposeNoScaling(rot, pos);
					relativePos += pos;
					relativeRot = relativeRot * rot;
					parentNode = parentNode->mParent;
				}

				eg::Object* parent = root;
				if(parentNode != NULL)
					parent = root->FindFirstWithName(parentNode->mName.C_Str());

				eg::Object* lightObj = new eg::Object(parent);
				lightObj->transform.position = glm::vec3(
					aLight->mPosition.x,
					aLight->mPosition.y,
					aLight->mPosition.z
				);
				lightObj->transform.position += VEC3D_TO_POINT3(relativePos);

				if(aLight->mType != aiLightSource_POINT)
				{
					// we need to compute the quaternion from the dir vector
					// https://gamedev.stackexchange.com/a/149010
					float angle = atan2(aLight->mDirection.x, aLight->mDirection.z);
					lightObj->transform.rotation = glm::quat(
						0,
						1.0f * sin(angle / 2),
						0,
						cos(angle / 2)
					);

					lightObj->transform.rotation *= glm::quat(
						relativeRot.x,
						relativeRot.y,
						relativeRot.z,
						relativeRot.w
					);
				}

				lightObj->AddComponent(light);
			}
		}*/

		return root;

		LOG_INFO << "Model " << filename << " loaded";
	}
}