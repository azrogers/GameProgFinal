#pragma once

#include <json.hpp>
#include "../Renderer.hpp"

namespace eg
{
	class Object;

	class Component
	{
	public:
		virtual void Update(float seconds) {};
		virtual void Render(eg::Renderer* renderer, eg::Renderer::RenderPass pass) {};

		void Render(eg::Renderer* renderer);

		/**
		 * Does this component influence the render order?
		 * If false (default), RenderOrder() is never called.
		 */
		virtual bool HasRenderOrder() const { return false; }
		virtual int RenderOrder() const { return 0; }

		friend class eg::Object;
		static friend void to_json(nlohmann::json& j, const Component& comp)
		{
			j = {
				{ "name", typeid(comp).name() },
				{ "passes", comp.renderPasses },
				{ "order", comp.RenderOrder() } };
		}
	protected:
		eg::Object *object = nullptr;
		Renderer::RenderPass renderPasses;

		Component(Renderer::RenderPass rp = Renderer::RenderPass::Object)
			: renderPasses(rp)
		{};
	};
};