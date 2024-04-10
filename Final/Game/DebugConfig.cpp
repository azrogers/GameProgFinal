#include "DebugConfig.hpp"

void game::DebugConfig::Draw(NVGcontext* context, int width, int height)
{
	std::stringstream text;
	text << "Wireframe [1]: " << (wireframe ? "On" : "Off") << std::endl;
	text << "FOV [2/3]: " << std::fixed << std::setprecision(1) << fov << std::endl;
	text << "Full Bright [4]: " << (fullBright ? "On" : "Off") << std::endl;
	text << "Debug Graphics [5]: " << (!hideDebug ? "On" : "Off") << std::endl;
	text << "Show Normals [6]: " << (normalOnly ? "True" : "False") << std::endl;
	text << "Show Fog [7]: " << (showFog ? "True" : "False");

	nvgFontFace(context, "sans-bold");
	nvgFontSize(context, 26.0f);

	nvgFillColor(context, nvgRGB(0, 0, 0));
	nvgFontBlur(context, 2);
	nvgTextBox(context, 10, 10, 400, text.str().c_str(), NULL);

	nvgFontBlur(context, 0);
	nvgFillColor(context, nvgRGB(255, 255, 255));
	nvgTextBox(context, 10, 10, 400, text.str().c_str(), NULL);
}

void game::DebugConfig::ApplyConfig(eg::RendererSettings* settings)
{
	settings->fov = fov;
	settings->fullBright = fullBright;
	settings->hideDebug = hideDebug;
	settings->wireframe = wireframe;
	settings->normalOnly = normalOnly;
	settings->showGui = showGui;
	settings->showFog = showFog;
}

void game::DebugConfig::Update(float seconds)
{
	if(input->IsKeyDown(SDL_SCANCODE_1))
		wireframe = !wireframe;
	if(input->IsKey(SDL_SCANCODE_2))
		fov -= seconds * 10;
	else if(input->IsKey(SDL_SCANCODE_3))
		fov += seconds * 10;
	else if(input->IsKeyDown(SDL_SCANCODE_4))
		fullBright = !fullBright;
	else if(input->IsKeyDown(SDL_SCANCODE_5))
		hideDebug = !hideDebug;
	else if(input->IsKeyDown(SDL_SCANCODE_6))
		normalOnly = !normalOnly;
	else if(input->IsKeyDown(SDL_SCANCODE_7))
		showFog = !showFog;
	else if(input->IsKeyDown(SDL_SCANCODE_GRAVE))
		showGui = !showGui;

	if(fov < 0)
		fov = 0;
}
