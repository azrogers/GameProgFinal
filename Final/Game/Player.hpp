#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <nanovg/nanovg.h>

#include "../Engine/Graphics/Vertex.hpp"
#include "../Engine/InputManager.hpp"
#include "GUI.hpp"

namespace game
{
	class Player : public GUIElement, public eg::Component
	{
	public:
		Player(eg::InputManager* input) : input(input) {};
		void Update(float deltaSeconds) override;

		glm::mat4 GetCameraMatrix();
		void Draw(NVGcontext *context, int width, int height) override;

		glm::vec3 playerMin, playerMax;
	private:
		bool isAiming = false;
		eg::InputManager* input;

		float rotationX = 0;
		float rotationY = 0;
	};
};