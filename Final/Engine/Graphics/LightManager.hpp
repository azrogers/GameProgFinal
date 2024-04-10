#pragma once

#include <unordered_set>
#include <vector>

namespace eg
{
	class Light;

	/**
	 * The LightManager handles uploading the data for all lights in the scene
	 * to the graphics card.
	 */
	class LightManager
	{
	public:
		/**
		 * Called by the Render() method of lights, to add a light to the scene.
		 */
		void AddLight(eg::Light* light);
		/**
		 * Uploads light data to this shader program.
		 */
		void Upload(eg::Program* shaderProgram);
		/**
		 * Clears data and prepares for next frame.
		 */
		void End();
	private:
		std::vector<eg::Light*> lights;
		std::unordered_set<eg::Program*> programs;
	};
};