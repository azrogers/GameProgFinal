#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "Display.hpp"

namespace eg
{
	class InputManager
	{
	public:
		void Update();

		// Returns if the key is currently down
		bool IsKey(int keyCode);
		// Returns if the key was pressed down this frame
		bool IsKeyDown(int keyCode);
		// Returns if the key stopped being pressed this frame
		bool IsKeyUp(int keyCode);

		void LockMouse();
		void UnlockMouse();

		void GetMousePosition(float* x, float* y);
		void GetNormalizedMousePosition(float * x, float * y);
		void GetAbsoluteMousePosition(int *x, int* y);
		glm::vec3 UnprojectMouse(glm::mat4 viewMatrix, glm::mat4 projMatrix);
		bool IsMouseButton(int code);
		bool IsMouseButtonDown(int code);
		bool IsMouseButtonUp(int code);
	private:
		InputManager(eg::Display* display) : windowHandle(display->GetWindowHandle()) {};

		SDL_Window* windowHandle;
		uint32_t prevMouseState;
		uint32_t currentMouseState;

		int numKeys = 0;
		uint8_t *currentKeyState = NULL;
		uint8_t *prevKeyState = NULL;

		friend class Display;
	};
};