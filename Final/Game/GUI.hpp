#pragma once

#include <vector>
#include <nanovg/nanovg.h>
#include <SDL2/SDL.h>

#include "../Engine/Engine.hpp"
#include "../Vendor/perf.h"

namespace game 
{
	class GUIElement
	{
	public:
		GUIElement() {}
		virtual void Draw(NVGcontext* context, int width, int height) = 0;
	};

	class GUI : public eg::Component
	{
	public:
		GUI(eg::Display* display);
		~GUI();
		void PreDraw();
		void Render(eg::Renderer* renderer, eg::Renderer::RenderPass pass) override;
		void Update(float frameTime) override;

		void AddElement(GUIElement* element);
	private:
		void renderGui();

		eg::Display* display;
		NVGcontext* context;

		int fontNormal, fontBold;
		uint32_t cpuTimeStart;

		PerfGraph fpsGraph, cpuGraph, gpuGraph;
		GPUtimer gpuTimer;

		std::vector<GUIElement *> elements;
	};
};