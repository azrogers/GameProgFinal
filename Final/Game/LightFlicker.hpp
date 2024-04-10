#pragma once

#include <random>

#include "../Engine/ECS/Component.hpp"
#include "../Engine/Graphics/Light.hpp"

namespace game
{
	class LightFlicker : public eg::Component
	{
	public:
		LightFlicker(eg::Light* light) : 
			randDist(std::uniform_real_distribution<double>(0.0, 5.0))
		{
			std::random_device rd;
			mt = std::mt19937(rd());

			max = randDist(mt);
			lights.push_back(light);
		}

		void Update(float seconds) override
		{
			counter += seconds;

			if(counter > max)
			{
				for(auto l : lights)
					l->brightness = lightOn ? 0 : 1;
				lightOn = !lightOn;
				counter = 0;
				max = randDist(mt);
			}
		}

		vector<eg::Light*> lights;
	private:
		std::uniform_real_distribution<double> randDist;
		std::mt19937 mt;
		float counter = 0;
		float max = 0;
		bool lightOn = true;
	};
};