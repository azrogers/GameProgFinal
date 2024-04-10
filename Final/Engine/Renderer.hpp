#pragma once

#include <glm/glm.hpp>
#include "Display.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/LightManager.hpp"

namespace eg
{
	class Object;

	struct RendererSettings
	{
		float fov = 75.0f;
		bool wireframe = false;
		bool fullBright = false;
		bool hideDebug = false;
		bool normalOnly = false;
		bool showGui = true;
		bool showFog = true;
	};

	class Renderer
	{
	public:
		enum class RenderPass : char
		{ 
			Light = 1, 
			Object = 2, 
			Debug = 4, 
			GUI = 8,
			Skybox = 16
		};

		friend inline RenderPass operator | (RenderPass a, RenderPass b)
		{
			return static_cast<RenderPass>(static_cast<char>(a) | static_cast<char>(b));
		}

		friend inline char operator & (RenderPass a, RenderPass b)
		{
			return static_cast<char>(a) & static_cast<char>(b);
		}

		~Renderer();
		void Begin(glm::mat4 cameraMatrix, glm::vec3 pos);
		void Render(eg::Object* root);
		void End();
		void SetupShader(eg::Program *);

		glm::mat4 CameraMatrix() { return cameraMatrix; }
		glm::mat4 ProjectionMatrix() { return projMatrix; }

		eg::Renderer::RenderPass CurrentPass() { return currentPass; }

		eg::LightManager* LightManager() { return &lightManager; }

		RendererSettings settings;
		float fogDensity;
		glm::vec3 fogColor;
	private:
		eg::Renderer::RenderPass currentPass = RenderPass::Object;
		Display* display;
		glm::mat4 cameraMatrix, projMatrix;
		glm::vec3 cameraPos;
		eg::LightManager lightManager;

		Renderer(Display* display);
		friend class Display;
	};
};