#pragma once

#include "../Graphics/Mesh.hpp"
#include "../Graphics/Material.hpp"
#include "Object.hpp"
#include "Component.hpp"

namespace eg
{
	class MeshRenderer : public Component
	{
	public:
		MeshRenderer(eg::Mesh* mesh, eg::Graphics::Material* mat) : mesh(mesh), mat(mat) {};
		void Render(eg::Renderer* renderer, eg::Renderer::RenderPass pass) override
		{
			mat->GetProgram()->Activate();
			mat->GetProgram()->Uniform<mat4>("transform") = object->WorldMatrix();
			renderer->SetupShader(mat->GetProgram());

			if(renderer->settings.fullBright)
				mat->GetProgram()->Uniform<GLint>("numLights") = 0;
			else
				renderer->LightManager()->Upload(mat->GetProgram());

			mat->SetupRender();
			mesh->Render();
			mat->CleanupRender();
		}

		void Update(float seconds) override {};

		bool HasRenderOrder() const override { return true; }
		int RenderOrder() const override
		{
			return mat->GetProgram()->GetShaderHash();
		}
		
	private:
		eg::Mesh* mesh;
		eg::Graphics::Material* mat;
	};
};