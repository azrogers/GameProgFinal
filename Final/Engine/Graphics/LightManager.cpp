#include "../ECS/Component.hpp"
#include "LightManager.hpp"
#include "Shader.hpp"
#include "Light.hpp"

namespace eg
{
	void LightManager::AddLight(eg::Light* light)
	{
		lights.push_back(light);
	}

	void LightManager::Upload(eg::Program* shaderProgram)
	{
		// don't upload twice in one frame
		if(programs.count(shaderProgram) > 0) return;

		for(int i = 0; i < lights.size(); i++)
		{
			lights.at(i)->Upload(shaderProgram, i);
		}

		shaderProgram->Uniform<GLint>("numLights") = lights.size();
	}

	void LightManager::End()
	{
		lights.clear();
		programs.clear();
	}
}