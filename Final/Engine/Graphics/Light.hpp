#pragma once

#include "Vertex.hpp"
#include "../Renderer.hpp"

namespace eg
{
	class Component;

	enum LightType
	{
		PointLight = 0,
		SpotLight = 1,
		DirectionalLight = 2
	};

	class Light : public Component
	{
	public:
		eg::Point3 color;
		eg::Point3 ambient;
		float brightness = 1.0f;

		void Render(eg::Renderer* renderer, eg::Renderer::RenderPass pass) override;
		void Upload(eg::Program *program, int num);

		static Light* CreatePointLight(
			eg::Point3 color, 
			eg::Point3 ambient, 
			float radius,
			float falloff);
		static Light* CreateSpotLight(
			eg::Point3 color, 
			eg::Point3 ambient, 
			float falloff, 
			float angle);
		static Light* CreateDirLight(
			eg::Point3 color,
			eg::Point3 ambient);
	private:
		Light() :
			Component(Renderer::RenderPass::Light)
		{};

		float falloff;
		float spotAngle;
		float radius;
		eg::Point3 dirDirection;
		int type = PointLight;
	};
};