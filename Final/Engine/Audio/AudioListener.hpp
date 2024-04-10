#pragma once

namespace eg
{
	class Component;

	class AudioListener : public Component
	{
	public:
		void Update(float seconds) override;

		float Gain() const { return gain; }
		void Gain(float gain) { this->gain = gain; }
	private:
		float gain = 0.8f;
	};
}