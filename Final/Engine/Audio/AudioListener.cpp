#include "../ECS/Component.hpp"
#include "../ECS/Object.hpp"
#include "AudioListener.hpp"
#include <Openal/al.h>
#include <glm/gtx/matrix_decompose.hpp>

namespace eg
{
	void AudioListener::Update(float seconds)
	{
		glm::vec3 position = this->object->transform.WorldPos();
		glm::vec3 velocity = this->object->transform.Velocity();

		alListener3f(AL_POSITION, position.x, position.y, position.z);
		alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
		glm::vec3 forwardVector = glm::vec3(0, 0, 1) * this->object->transform.WorldRot();
		float orientation[] = { forwardVector.x, forwardVector.y, forwardVector.z, 0, 1, 0 };
		alListenerfv(AL_ORIENTATION, orientation);
		alListenerf(AL_GAIN, gain);
	}
}