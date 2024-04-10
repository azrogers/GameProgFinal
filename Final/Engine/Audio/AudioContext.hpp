#pragma once

#include <OpenAL/alc.h>

namespace eg
{
	class AudioContext
	{
	public:
		AudioContext();
		~AudioContext();
	private:
		ALCdevice * device;
		ALCcontext* context;
	};
}