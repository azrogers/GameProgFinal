#include "Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>
#define DEBUG_DRAW_IMPLEMENTATION
#include "../Vendor/DebugDraw.hpp"
#include "ECS/Object.hpp"

eg::DDRenderInterfaceCoreGL* ddInterface;

namespace eg
{
	Renderer::Renderer(Display* display) : display(display)
	{
		ddInterface = new eg::DDRenderInterfaceCoreGL(display);
		dd::initialize(ddInterface);
	}

	Renderer::~Renderer()
	{
		dd::shutdown();
		delete ddInterface;
	}

	void Renderer::Begin(glm::mat4 cameraMatrix, glm::vec3 pos)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, settings.wireframe ? GL_LINE : GL_FILL);

		this->cameraMatrix = cameraMatrix;
		this->projMatrix = glm::perspective(glm::radians(settings.fov), display->GetAspectRatio(), 0.01f, 1000.0f);
		cameraPos = pos;
	}

	void Renderer::Render(eg::Object* root)
	{
		currentPass = RenderPass::Light;
		root->Render(this);

		// skybox renders first, so that it's behind all objects
		currentPass = RenderPass::Skybox;
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		root->Render(this);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);

		currentPass = RenderPass::Object;
		root->Render(this);

		currentPass = RenderPass::Debug;
		// update the view-projection matrix of debug-draw so things get placed in the world
		// instead of just in our vision
		mat4 tempVpMatrix = projMatrix * cameraMatrix;
		Vectormath::SSE::Matrix4 ddVpMatrix;
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				ddVpMatrix.setElem(i, j, tempVpMatrix[i][j]);
			}
		}
		ddInterface->mvpMatrix = ddVpMatrix;

		if(settings.hideDebug)
			dd::clear();
		dd::flush(SDL_GetTicks());

		// draw gui normally even if in wireframe
		currentPass = RenderPass::GUI;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if(settings.showGui)
			root->Render(this);

		lightManager.End();
	}

	void Renderer::End()
	{
		display->Swap();
	}

	void Renderer::SetupShader(eg::Program *program)
	{
		if(fogDensity > 0)
		{
			program->Uniform<float>("fogDensity") = settings.showFog ? fogDensity : 0;
			program->Uniform<vec3>("fogColor") = fogColor;
		}

		program->Uniform<GLint>("wireframe") = settings.wireframe;
		program->Uniform<GLint>("normalOnly") = settings.normalOnly;
		program->Uniform<mat4>("camera") = cameraMatrix;
		program->Uniform<mat4>("projection") = projMatrix;
		program->Uniform<vec3>("cameraPos") = cameraPos;
		program->Uniform<float>("time") = SDL_GetTicks() / 1000.0f;
	}
};