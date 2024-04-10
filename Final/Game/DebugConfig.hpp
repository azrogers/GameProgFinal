#pragma once

#include <sstream>
#include <nanovg/nanovg.h>
#include "../Engine/ECS/Component.hpp"
#include "../Engine/InputManager.hpp"
#include "GUI.hpp"

namespace game
{
	class DebugConfig : public GUIElement, public eg::Component
	{
	public:
		DebugConfig(eg::InputManager* input) : input(input) {};

		bool wireframe = false;
		bool fullBright = false;
		bool hideDebug = true;
		bool normalOnly = false;
		bool showGui = false;
		bool showFog = true;
		float fov = 75.0f;

		void Draw(NVGcontext* context, int width, int height) override;

		void ApplyConfig(eg::RendererSettings* settings);

		void Update(float seconds) override;
	private:
		eg::InputManager* input;
	};
}