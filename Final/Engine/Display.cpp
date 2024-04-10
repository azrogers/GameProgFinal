#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Display.hpp"
#include "SDL.hpp"
#include "InputManager.hpp"
#include "Audio/AudioContext.hpp"
#include "Renderer.hpp"

namespace eg
{
	Display::Display(unsigned int width, unsigned int height) : currentWidth(width), currentHeight(height)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		SDL_DisplayMode dm;
		SDL_GetDesktopDisplayMode(0, &dm);
		display = SDL_CreateWindow(
			"Game Programming I Final",
			dm.w / 2 - width / 2, dm.h / 2 - height / 2,
			width,
			height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		if(!display) throw sdl::Exception("Failed to create window");
		if(!SDL_GL_CreateContext(display)) throw sdl::Exception("Failed to create context");

	#ifndef __APPLE__
		glewExperimental = GL_TRUE;
		glewInit();
		glGetError();
	#endif

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, glm::value_ptr(glm::vec4(1.0f)));

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_CULL_FACE);

		/* Setup our viewport. */
		glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

		input = new eg::InputManager(this);
		renderer = new eg::Renderer(this);
		audioContext = new eg::AudioContext();
	}

	Display::~Display()
	{
		delete input, renderer, audioContext;
	}

	void Display::Swap() const
	{
		SDL_GL_SwapWindow(display);
	}

	void Display::PollEvents()
	{
		/* Our SDL event placeholder. */
		SDL_Event event;

		/* Grab all the events off the queue. */
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_WINDOWEVENT:
				if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					Resize(event.window.data1, event.window.data2);
				}
				break;
			case SDL_QUIT:
				/* Handle quit requests (like Ctrl-c). */
				running = false;
				break;
			}
		}

		input->Update();
	}

	void Display::Resize(int newWidth, int newHeight)
	{
		currentWidth = newWidth;
		currentHeight = newHeight;
		glViewport(0, 0, static_cast<GLsizei>(newWidth), static_cast<GLsizei>(newHeight));
	}
}