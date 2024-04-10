#include "../ECS/Object.hpp"
#include "../ECS/Component.hpp"
#include "AudioSource.hpp"
#include <OpenAL/al.h>
#include <plog/Log.h>

#define BUFFER_COUNT 4

namespace eg
{
	AudioSource::AudioSource()
	{
		alGenSources(1, &sourceId);
		RequeueBuffers();
	}

	AudioSource::~AudioSource()
	{
		alDeleteSources(1, &sourceId);
	}

	void AudioSource::Play()
	{
		alSourcePlay(sourceId);
		state = AudioSourceState::Playing;
	}

	void AudioSource::Pause()
	{
		alSourceStop(sourceId);
		state = AudioSourceState::Paused;
	}

	void AudioSource::RequeueBuffers()
	{
		if(clip == nullptr) return;

		if(clip->IsStreaming())
		{
			unsigned int buffers[BUFFER_COUNT];
			int genCount = 0;
			for(int i = 0; i < BUFFER_COUNT && clip->State() == AudioClipState::Playing; i++)
			{
				if(clip->GetBuffer(buffers + i))
					genCount++;
			}
			alSourceQueueBuffers(sourceId, genCount, buffers);
		}
		else
		{
			unsigned int buffer;
			if(clip->GetBuffer(&buffer))
				alSourceQueueBuffers(sourceId, 1, &buffer);
		}
	}

	void AudioSource::Update(float seconds)
	{
		glm::vec3 position = this->object->transform.WorldPos();
		glm::vec3 velocity = this->object->transform.Velocity();

		alSource3f(sourceId, AL_POSITION, position.x, position.y, position.z);
		alSource3f(sourceId, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
		alSourcef(sourceId, AL_REFERENCE_DISTANCE, refDistance);
		alSourcef(sourceId, AL_MAX_DISTANCE, maxDistance);
		alSourcef(sourceId, AL_GAIN, gain);

		if(state != AudioSourceState::Playing || clip == nullptr) return;

		int sourceState;
		alGetSourcei(sourceId, AL_SOURCE_STATE, &sourceState);
		if(clip->State() == AudioClipState::Ended && sourceState == AL_STOPPED)
		{
			alSourceStop(sourceId);
			state = AudioSourceState::Ended;

			if(clip->Looping())
			{
				state = AudioSourceState::Playing;
				int bufferCount = 0;
				alGetSourcei(sourceId, AL_BUFFERS_QUEUED, &bufferCount);
				unsigned int trash = 0;
				alSourceUnqueueBuffers(sourceId, bufferCount, &trash);
				clip->Reset();
				RequeueBuffers();
				alSourcePlay(sourceId);
			}
		}
		else if(sourceState == AL_STOPPED && clip->State() == AudioClipState::Playing)
		{
			RequeueBuffers();
			alSourcePlay(sourceId);
		}

		int buffersProcessed = 0;
		alGetSourcei(sourceId, AL_BUFFERS_PROCESSED, &buffersProcessed);
		while(buffersProcessed > 0)
		{
			unsigned int buffer;
			alSourceUnqueueBuffers(sourceId, 1, &buffer);
			if(clip->State() == AudioClipState::Ended) break;
			clip->FillBuffer(buffer);
			alSourceQueueBuffers(sourceId, 1, &buffer);
			buffersProcessed--;
		}

		ALenum error;
		while((error = alGetError()) != AL_NO_ERROR)
			LOG_ERROR << "OpenAL error: " << error;
	}
}