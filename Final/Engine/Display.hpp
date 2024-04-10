#pragma once

#include <SDL2/SDL.h>

namespace eg
{
	class InputManager;
	class Renderer;
	class AudioContext;

	class Display
	{
	public:
		Display(unsigned int width, unsigned int height);
		~Display();
		void Swap(void) const;
		void PollEvents();
		SDL_Window* GetWindowHandle() const { return display; }
		float GetAspectRatio() { return (float)currentWidth / currentHeight; }
		void GetWindowSize(int *w, int *h) { *w = currentWidth; *h = currentHeight; }
		void Resize(int newWidth, int newHeight);
		bool IsRunning() { return running; }

		eg::InputManager *InputManager() { return input; }
		eg::Renderer *Renderer() { return renderer; }
	private:
		SDL_Window* display;
		bool running = true;
		unsigned int currentWidth, currentHeight;
		eg::InputManager* input;
		eg::Renderer* renderer;
		eg::AudioContext* audioContext;
	};
};