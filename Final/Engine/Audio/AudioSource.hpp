#pragma once
#include "AudioClip.hpp"

namespace eg
{
	enum class AudioSourceState
	{
		Stopped,
		Paused,
		Playing,
		Ended
	};

	class Component;

	class AudioSource : public Component
	{
	public:
		AudioSource();
		~AudioSource();

		void Play();
		void Pause();

		void Update(float seconds) override;

		AudioSourceState State() const { return state; }

		eg::AudioClip* Clip() const { return clip; }
		void Clip(eg::AudioClip* clip) { this->clip = clip; RequeueBuffers(); }

		float gain = 1.0f;
		float maxDistance = 150.0f;
		float refDistance = 50.0f;
	private:
		eg::AudioClip* clip = nullptr;
		void RequeueBuffers();
		unsigned int sourceId;
		AudioSourceState state = AudioSourceState::Stopped;
	};
};