#pragma once

#include "../Engine/ECS/Object.hpp"
#include "../Engine/ECS/Component.hpp"

namespace game
{
	class CarController : public eg::Component
	{
	public:
		void Update(float seconds) override;

		float wheelSpeed = 1.0f, carSpeed = 1.0f;
		float direction = 1.0f;
		eg::Object* flWheel;
		eg::Object* frWheel;
		eg::Object* blWheel;
		eg::Object* brWheel;
	};
};