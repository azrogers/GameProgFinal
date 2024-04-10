#pragma once

#include <string>
#include <sndfile.hpp>

namespace eg
{
	enum class AudioClipState
	{
		Ended,
		Playing,
		Error
	};

	class AudioClip
	{
	public:
		AudioClip(std::string filename, bool streaming = true);
		~AudioClip();

		AudioClipState State() const { return state; }
		bool IsStreaming() const { return streaming; }
		bool GetBuffer(unsigned int* buffer);
		void FillBuffer(unsigned int buffer);
		void Reset();

		void Looping(bool looping) { this->looping = looping; }
		bool Looping() const { return looping; }
	private:
		bool looping = false;
		bool streaming;
		AudioClipState state = AudioClipState::Playing;
		SndfileHandle* handle;
		short* data = nullptr;
		int format;
		int sampleRate, channels, frames;
	};
};