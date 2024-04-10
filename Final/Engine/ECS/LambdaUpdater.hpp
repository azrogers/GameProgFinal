#pragma once 

#include <functional>
#include "Component.hpp"

namespace eg
{
	class LambdaUpdater : public Component
	{
	public:
		LambdaUpdater(std::function<void(eg::Object*, float)> updateFunction) :
			func(updateFunction),
			Component()
		{};

		void Update(float seconds) override
		{
			func(this->object, seconds);
		}
	private:
		std::function<void(eg::Object*, float)> func;
	};
}
