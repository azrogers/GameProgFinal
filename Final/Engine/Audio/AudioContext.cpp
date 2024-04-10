#include "AudioContext.hpp"
#include <plog/Log.h>
#include <OpenAL/al.h>

namespace eg
{
	AudioContext::AudioContext()
	{
		device = alcOpenDevice(nullptr);
		context = alcCreateContext(device, nullptr);
		alcMakeContextCurrent(context);
		if(!context)
		{
			LOG_ERROR << "couldn't initialize OpenAL context";
		}

		alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	}

	AudioContext::~AudioContext()
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		alcCloseDevice(device);
	}
}