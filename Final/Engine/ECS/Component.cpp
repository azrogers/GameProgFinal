#include "Object.hpp"
#include "Component.hpp"

namespace eg
{
	void Component::Render(eg::Renderer* renderer)
	{
		if(renderPasses & renderer->CurrentPass())
			Render(renderer, renderer->CurrentPass());
	}
};