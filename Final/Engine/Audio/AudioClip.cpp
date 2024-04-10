#include <OpenAL/al.h>
#include <plog/Log.h>
#include "AudioClip.hpp"

#define BUFFER_SIZE 4096

namespace eg
{
	AudioClip::AudioClip(std::string filename, bool streaming) : streaming(streaming)
	{
		handle = new SndfileHandle(filename, SFM_READ);

		channels = handle->channels();
		if(channels == 2)
			format = AL_FORMAT_STEREO16;
		else if(channels == 1)
			format = AL_FORMAT_MONO16;
		else
			throw new std::exception("unsupported number of channels");
		sampleRate = handle->samplerate();
		frames = handle->frames();

		LOG_INFO << "loaded file " << filename << ": " << handle->getString(SF_STR_TITLE);

		if(!streaming)
		{
			data = new short[handle->frames() * channels];
			handle->readf(data, handle->frames());
			delete handle;
		}
	}

	AudioClip::~AudioClip()
	{
		if(data != nullptr)
			delete[] data;
		delete handle;
	}

	bool AudioClip::GetBuffer(unsigned int* buffer)
	{
		if(state == AudioClipState::Playing)
		{
			alGenBuffers(1, buffer);
			FillBuffer(*buffer);
			return true;
		}

		return false;
	}

	void AudioClip::FillBuffer(unsigned int buffer)
	{
		if(streaming)
		{
			static short tempData[BUFFER_SIZE];
			static short* testPtr = tempData;
			int numRead = sf_read_short(handle->rawHandle(), testPtr, BUFFER_SIZE);
			if(numRead < BUFFER_SIZE)
				state = AudioClipState::Ended;
			alBufferData(buffer, format, tempData, BUFFER_SIZE * sizeof(short), sampleRate);
		}
		else
		{
			alBufferData(buffer, format, data, frames * channels * sizeof(short), sampleRate);
			state = AudioClipState::Ended;
		}
	}

	void AudioClip::Reset()
	{
		if(streaming)
			handle->seek(0, SEEK_SET);
		state = AudioClipState::Playing;
	}
};