#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../OpenGL.hpp"
#include <json.hpp>
using json = nlohmann::json;

namespace eg
{
	class Object;

	class Transform
	{
	public:
		Transform(Object *parent) : parent(parent) {};

		glm::mat4 Matrix() const;
		glm::vec3 EulerAngles() const;
		void EulerAngles(glm::vec3 angles);

		void Rotate(float angle, glm::vec3 axis);
		void Translate(glm::vec3 distance);

		void Update(float seconds);

		glm::vec3 Velocity() const { return velocity; }
		glm::vec3 WorldPos() const { return worldPos; }
		glm::quat WorldRot() const { return worldRot; }

		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale = glm::vec3(1.0f);

		static Transform Add(const Transform& a, const Transform& b);
	private:
		glm::vec3 worldPos;
		glm::quat worldRot;

		eg::Object *parent;
		glm::vec3 lastFramePos;
		glm::vec3 velocity;
		bool updateRunOnce = false;
	};

	static void to_json(json& j, const Transform& transform)
	{
		glm::vec3 rotVec = glm::eulerAngles(transform.rotation);
		j = {
			{ "position", transform.position },
			{ "rotation", rotVec },
			{ "scale", transform.scale }
		};
	}
};