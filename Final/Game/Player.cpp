#include "Player.hpp"

#include <plog/Log.h>
#include <sstream>
#include <SDL2/SDL.h>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

#define MOUSE_SENSITIVITY 15.0f
#define WALK_SPEED 0.25f
#define SPRINT_SPEED 0.75f
#define MIN_Y_LOOK glm::radians(-60.0f)
#define MAX_Y_LOOK glm::radians(60.0f)

namespace game
{
	void Player::Update(float deltaSeconds)
	{
		if(input->IsKeyDown(SDL_SCANCODE_F) && !isAiming)
		{
			LOG_DEBUG << "Aim start";
			input->LockMouse();
			isAiming = true;
		}
		else if(
			(input->IsKeyDown(SDL_SCANCODE_F) || input->IsKeyDown(SDL_SCANCODE_ESCAPE)) &&
			isAiming)
		{
			LOG_DEBUG << "Aim end";
			input->UnlockMouse();
			isAiming = false;
		}

		if(isAiming)
		{
			// rotate according to mouse
			float x, y;
			input->GetMousePosition(&x, &y);

			rotationX += x * deltaSeconds * MOUSE_SENSITIVITY;
			rotationY += y * deltaSeconds * MOUSE_SENSITIVITY;

			// if the player looks too far down they'll flip themselves over
			// so we limit them
			if(rotationY < MIN_Y_LOOK)
				rotationY = MIN_Y_LOOK;
			else if(rotationY > MAX_Y_LOOK)
				rotationY = MAX_Y_LOOK;
		}

		// rotate on each axis seperately instead of compounding rotations on one
		// this prevents accidental roll
		glm::quat rotationXQuat = glm::angleAxis(rotationX, vec3(0, 1, 0));
		glm::quat rotationYQuat = glm::angleAxis(rotationY, vec3(1, 0, 0));
		object->transform.rotation = rotationYQuat * rotationXQuat;

		// move the player
		glm::vec3 newPosition;
		float speed = input->IsKey(SDL_SCANCODE_LSHIFT) ? SPRINT_SPEED : WALK_SPEED;
		if(input->IsKey(SDL_SCANCODE_W))
		{
			newPosition += glm::vec3(0, 0, speed);
		}
		if(input->IsKey(SDL_SCANCODE_S))
		{
			newPosition -= glm::vec3(0, 0, speed);
		}
		if(input->IsKey(SDL_SCANCODE_A))
		{
			newPosition += glm::vec3(speed, 0, 0);
		}
		if(input->IsKey(SDL_SCANCODE_D))
		{
			newPosition -= glm::vec3(speed, 0, 0);
		}

		// rotate the movement vector by the rotation of the camera, to move
		// relative to the player's current rotation
		object->transform.position += newPosition * object->transform.rotation;
		if(playerMin.x != 0 && playerMin.y != 0 && playerMin.z != 0)
			object->transform.position = glm::vec3(
				glm::clamp(object->transform.position.x, playerMin.x, playerMax.x),
				glm::clamp(object->transform.position.y, playerMin.y, playerMax.y),
				glm::clamp(object->transform.position.z, playerMin.z, playerMax.z)
			);
	}


	glm::mat4 Player::GetCameraMatrix()
	{
		return glm::translate(
			glm::mat4_cast(glm::normalize(object->transform.rotation)), 
			object->transform.position);
	}

	void Player::Draw(NVGcontext *context, int width, int height)
	{
		std::stringstream text;
		text << "Position: " <<
			"(" << object->transform.position.x << ", " <<
			object->transform.position.y << ", " <<
			object->transform.position.z << ")" << endl;
		text << "Rotation: " << "(" << rotationX << ", " << rotationY << ")";

		nvgFontFace(context, "sans");
		nvgFontSize(context, 18.0f);
		nvgTextAlign(context, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);

		nvgFillColor(context, nvgRGB(0, 0, 0));
		nvgFontBlur(context, 4);
		nvgTextBox(context, 10, height - 20, 500, text.str().c_str(), NULL);

		nvgFillColor(context, nvgRGB(255, 255, 255));
		nvgFontBlur(context, 0);
		nvgTextBox(context, 10, height - 20, 500, text.str().c_str(), NULL);
	}
}