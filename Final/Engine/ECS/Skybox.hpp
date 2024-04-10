#pragma once

#include "Component.hpp"
#include "../Graphics/Texture.hpp"
#include "../Graphics/MeshPrimitives.hpp"
#include "../OpenGL.hpp"

namespace eg
{
	class Skybox : public Component
	{
	public:
		Skybox(eg::Texture* skyboxTexture, eg::Program* skyboxShader) : 
			texture(skyboxTexture), 
			shader(skyboxShader),
			Component(eg::Renderer::RenderPass::Skybox)
		{
			mesh = MeshPrimitives::Cube();
		}

		void Render(eg::Renderer* renderer, eg::Renderer::RenderPass pass) override
		{
			shader->Activate();
			renderer->SetupShader(shader);
			
			// remove translation from view matrix
			glm::mat4 viewMat = shader->Uniform<glm::mat4>("camera");
			viewMat = glm::mat4(glm::mat3(viewMat));
			shader->Uniform<glm::mat4>("camera") = viewMat;
			shader->Uniform<GLint>("skybox") = texture->Activate();

			mesh->Render();

			texture->Deactivate();
		}
	private:
		eg::Texture* texture;
		eg::Program* shader;
		eg::Mesh* mesh;
	};
}