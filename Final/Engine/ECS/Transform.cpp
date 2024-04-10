#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "Object.hpp"
#include "Transform.hpp"

namespace eg
{
	glm::mat4 Transform::Matrix() const
	{
		glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), scale);
		glm::mat4 translateMat = glm::translate(glm::mat4(1.0), position);
		glm::mat4 rotateMat = glm::mat4_cast(glm::normalize(rotation));

		return translateMat * rotateMat * scaleMat;
	}

	glm::vec3 Transform::EulerAngles() const
	{
		return glm::eulerAngles(rotation);
	}

	void Transform::EulerAngles(glm::vec3 angles)
	{
		rotation = glm::quat(angles);
	}

	void Transform::Rotate(float angle, glm::vec3 axis)
	{
		rotation = glm::rotate(rotation, angle, axis);
	}

	void Transform::Translate(glm::vec3 distance)
	{
		position += distance;
	}

	void Transform::Update(float seconds)
	{
		glm::vec3 scale, translation, skew;
		glm::vec4 perspective;
		glm::decompose(this->parent->WorldMatrix(), scale, worldRot, worldPos, skew, perspective);

		if(!updateRunOnce)
		{
			lastFramePos = position;
			updateRunOnce = true;
			return;
		}

		velocity = worldPos - lastFramePos;
	}

	eg::Transform Transform::Add(const Transform& a, const Transform& b)
	{
		Transform t = Transform(a.parent);
		t.position = a.position + b.position;
		t.rotation = a.rotation * b.rotation;
		t.scale = a.scale * b.scale;

		return t;
	}

};