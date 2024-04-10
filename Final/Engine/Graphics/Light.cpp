#include <debug_draw.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "../ECS/Object.hpp"
#include "../ECS/Component.hpp"
#include "Light.hpp"
#include "Shader.hpp"
#include "../Renderer.hpp"

namespace eg
{
	void Light::Render(eg::Renderer* renderer, eg::Renderer::RenderPass pass)
	{
		renderer->LightManager()->AddLight(this);

		glm::vec3 position = object->transform.WorldPos();
		glm::vec3 direction = object->transform.WorldRot() * glm::vec3(0, 1, 0);

		if(type == PointLight)
		{
			dd::sphere(VEC3_TO_DDVEC3(position), VEC3_TO_DDVEC3(color), radius);
		}
		else if(type == SpotLight)
		{
			glm::vec3 dir = glm::normalize(direction) * 1000.0f;
			glm::vec3 pos = position - dir;
			// http://mathcentral.uregina.ca/qq/database/qq.09.07/s/marija1.html
			// r = h * tan(X)
			float realRadius = glm::tan(glm::radians(spotAngle)) * 1000.0f;
			float realInnerRadius = glm::tan(glm::radians((1 - falloff) * spotAngle)) * 1000.0f;
			dd::cone(
				VEC3_TO_DDVEC3(dir),
				VEC3_TO_DDVEC3(pos),
				VEC3_TO_DDVEC3(color),
				0,
				realRadius);
			dd::cone(
				VEC3_TO_DDVEC3(dir),
				VEC3_TO_DDVEC3(pos),
				VEC3_TO_DDVEC3(color),
				0,
				realInnerRadius);
		}
		else if(type == DirectionalLight)
		{
			glm::vec3 from = glm::normalize(direction) * 1000.0f;
			glm::vec3 to = glm::vec3(0, 0, 0);
			dd::arrow(
				VEC3_TO_DDVEC3(from),
				VEC3_TO_DDVEC3(to),
				VEC3_TO_DDVEC3(color),
				1.0f
			);
		}

		dd::point(VEC3_TO_DDVEC3(position), VEC3_TO_DDVEC3(color), 15.0f);
	}

	void Light::Upload(eg::Program *program, int num)
	{
		glm::vec3 position = object->transform.WorldPos();
		glm::vec3 direction = object->transform.WorldRot() * glm::vec3(0, 1, 0);

		eg::LightUniform* uniforms = program->GetLightUniforms()[num];
		uniforms->position = position;
		uniforms->color = color * brightness;
		uniforms->ambient = ambient;
		uniforms->falloff = falloff;
		uniforms->type = type;
		uniforms->direction = direction;

		if(type == SpotLight)
		{
			uniforms->spotAngle = glm::cos(glm::radians(spotAngle));
			uniforms->innerAngle = glm::cos(glm::radians((1 - falloff) * spotAngle));
		}
		else if(type == PointLight)
		{
			uniforms->radius = radius;
		}
	}

	eg::Light* Light::CreatePointLight(
		eg::Point3 color,
		eg::Point3 ambient,
		float radius,
		float falloff)
	{
		eg::Light* light = new eg::Light();
		light->color = color;
		light->ambient = ambient;
		light->falloff = falloff;
		light->radius = radius;
		light->type = PointLight;

		return light;
	}

	eg::Light* Light::CreateSpotLight(
		eg::Point3 color,
		eg::Point3 ambient,
		float falloff,
		float angle)
	{
		eg::Light* light = new eg::Light();
		light->color = color;
		light->ambient = ambient;
		light->falloff = falloff;
		light->spotAngle = angle;
		light->type = SpotLight;

		return light;
	}

	eg::Light* Light::CreateDirLight(
		eg::Point3 color,
		eg::Point3 ambient
	)
	{
		eg::Light* light = new eg::Light();
		light->color = color;
		light->ambient = ambient;
		light->type = DirectionalLight;

		return light;
	}
}