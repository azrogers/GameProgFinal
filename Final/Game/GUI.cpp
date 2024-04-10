#include "GUI.hpp"

#include <GL/glew.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg/nanovg_gl.h>

#include "../Engine/Graphics/Texture.hpp"

#define GRAPH_WIDTH 200

namespace game
{
	GUI::GUI(eg::Display* display) : Component(eg::Renderer::RenderPass::GUI), display(display)
	{
		context = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
		if(context == NULL)
		{
			throw "Could not init nanovg.";
			return;
		}

		fontNormal = nvgCreateFont(context, "sans", "resource/OpenSans-Regular.ttf");
		fontBold = nvgCreateFont(context, "sans-bold", "resource/OpenSans-Bold.ttf");

		initGraph(&fpsGraph, GRAPH_RENDER_FPS, "FPS");
		initGraph(&cpuGraph, GRAPH_RENDER_MS, "CPU Time");
		initGraph(&gpuGraph, GRAPH_RENDER_MS, "GPU Time");

		initGPUTimer(&gpuTimer);
	}

	GUI::~GUI()
	{
		nvgDeleteGL3(context);
	}

	void GUI::PreDraw()
	{
		startGPUTimer(&gpuTimer);
		cpuTimeStart = SDL_GetTicks();
	}

	void GUI::Update(float frameTime)
	{
		updateGraph(&fpsGraph, frameTime);
	}

	void GUI::Render(eg::Renderer* renderer, eg::Renderer::RenderPass pass)
	{
		int w, h;
		display->GetWindowSize(&w, &h);

		nvgBeginFrame(context, w, h, (float)w / (float)h);

		renderGui();

		renderGraph(context, w - GRAPH_WIDTH - 5, 5, &fpsGraph);
		renderGraph(context, w - GRAPH_WIDTH * 2 - 5 * 2, 5, &cpuGraph);
		if(gpuTimer.supported)
			renderGraph(context, w - GRAPH_WIDTH * 3 - 5 * 3, 5, &gpuGraph);

		nvgEndFrame(context);

		updateGraph(&cpuGraph, (SDL_GetTicks() - cpuTimeStart) / 1000.0f);
		float gpuTimes[3];
		int n = stopGPUTimer(&gpuTimer, gpuTimes, 3);
		for(int i = 0; i < n; i++)
			updateGraph(&gpuGraph, gpuTimes[i]);

		// clean up dirty state
		glEnable(GL_DEPTH_TEST);
		eg::Texture::Deactivate();
	}

	void GUI::AddElement(GUIElement *elem)
	{
		elements.push_back(elem);
	}

	void GUI::renderGui()
	{
		int w, h;
		display->GetWindowSize(&w, &h);
		nvgTextAlign(context, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

		for(auto elem : elements)
		{
			nvgSave(context);
			elem->Draw(context, w, h);
			nvgRestore(context);
		}
	}
};