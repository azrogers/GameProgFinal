#pragma once

#include "GUI.hpp"
#include "DebugConfig.hpp"
#include "Player.hpp"
#include "../Engine/Engine.hpp"

class Project
{
public:
	Project(unsigned int width, unsigned int height);
	int Run(void);
protected:
	void Render(void);
	void ApplyTime(float seconds);

	eg::Display display;
	eg::Program phong;
	eg::Program skyboxShader;
	game::GUI* gui;
	game::DebugConfig* debugConfig;
	eg::InputManager *input;
	eg::Renderer *renderer;
	eg::Object* l1obj;
	eg::Object* l2obj;
	eg::Object* spotobj;
	eg::Object *scene;
	
	eg::Object* playerObj;
	game::Player* player;
};