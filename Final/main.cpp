//
//  GL1.cpp
//  GLProject1
//
//  Created by Nevin Flanagan on 1/9/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>
#include <SDL2/SDL.h>

#include "Engine/SDL.hpp"
#include "Game/Project.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
	plog::init(plog::debug, "logs.txt").addAppender(&consoleAppender);

	LOG_INFO << "Logging enabled.";

	static_cast<void>(argc), static_cast<void>(argv);

	try
	{
		sdl::Library SDL2(SDL_INIT_VIDEO);
		return Project(800, 600).Run();
	}
	catch(std::exception& e)
	{
		LOG_ERROR << e.what() << endl;
		cin.get();
		return 1;
	}
}
