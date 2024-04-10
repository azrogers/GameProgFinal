#include <SDL2/SDL.h>
#include <memory.h>
#include <glm/gtx/matrix_decompose.hpp>
#include "InputManager.hpp"

#define KEY_UP 0
#define KEY_DOWN 1

namespace eg
{
	void InputManager::Update()
	{
		// we've been here before, so copy to prev state
		if(numKeys != 0)
		{
			if(prevKeyState == NULL)
				prevKeyState = new Uint8[numKeys];
			memcpy(prevKeyState, currentKeyState, numKeys * sizeof(Uint8));
		}

		// the sdl array is updated as soon as a key is pressed, so we need to
		// copy it so it doesn't change until next frame
		const Uint8* tempCurrentState = SDL_GetKeyboardState(&numKeys);
		if(currentKeyState == NULL)
			currentKeyState = new Uint8[numKeys];
		memcpy(currentKeyState, tempCurrentState, numKeys * sizeof(Uint8));

		prevMouseState = currentMouseState;
		currentMouseState = SDL_GetMouseState(NULL, NULL);
	}

	bool InputManager::IsKey(int keyCode)
	{
		if(numKeys == 0) return false;

		return currentKeyState[keyCode] == KEY_DOWN;
	}

	bool InputManager::IsKeyDown(int keyCode)
	{
		if(numKeys == 0) return false;

		return currentKeyState[keyCode] == KEY_DOWN &&
			prevKeyState[keyCode] == KEY_UP;
	}

	bool InputManager::IsKeyUp(int keyCode)
	{
		return !IsKeyDown(keyCode);
	}

	void InputManager::LockMouse()
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	void InputManager::UnlockMouse()
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	void InputManager::GetMousePosition(float *x, float* y)
	{
		int tempX, tempY;
		SDL_GetRelativeMouseState(&tempX, &tempY);

		*x = tempX / 100.0;
		*y = tempY / 100.0;
	}

	void InputManager::GetNormalizedMousePosition(float *x, float * y)
	{
		int tempX, tempY, w, h;
		SDL_GetMouseState(&tempX, &tempY);
		SDL_GetWindowSize(windowHandle, &w, &h);

		*x = (2.0f * tempX) / w - 1.0f;
		*y = 1.0f - (2.0f * tempY) / h;
	}

	void InputManager::GetAbsoluteMousePosition(int *x, int *y)
	{
		SDL_GetMouseState(x, y);
	}

	glm::vec3 InputManager::UnprojectMouse(glm::mat4 viewMatrix, glm::mat4 projMatrix)
	{
		int x, y, w, h;
		GetAbsoluteMousePosition(&x, &y);
		SDL_GetWindowSize(windowHandle, &w, &h);
		glm::vec4 viewport = glm::vec4(0, 0, w, h);
		glm::vec3 coord = glm::vec3(x, h - y - 1, 0.0f);
		return glm::unProject(coord, viewMatrix, projMatrix, viewport);
	}

	bool InputManager::IsMouseButton(int code)
	{
		return (currentMouseState & code) == 1;
	}

	bool InputManager::IsMouseButtonDown(int code)
	{
		return (currentMouseState & code) == 1 && (prevMouseState & code) == 0;
	}

	bool InputManager::IsMouseButtonUp(int code)
	{
		return !InputManager::IsMouseButtonDown(code);
	}
};