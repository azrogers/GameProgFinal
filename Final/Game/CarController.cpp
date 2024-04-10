#include "CarController.hpp"

void game::CarController::Update(float seconds)
{
	flWheel->transform.Rotate(seconds * wheelSpeed, glm::vec3(1, 0, 0));
	frWheel->transform.Rotate(seconds * wheelSpeed, glm::vec3(1, 0, 0));
	blWheel->transform.Rotate(seconds * wheelSpeed, glm::vec3(1, 0, 0));
	brWheel->transform.Rotate(seconds * wheelSpeed, glm::vec3(1, 0, 0));

	object->transform.Translate(vec3(direction * seconds * carSpeed, 0, 0));
	if(direction > 0 && object->transform.position.x > 150)
		object->transform.position.x = -150;
	else if(direction < 0 && object->transform.position.x < -150)
		object->transform.position.x = 150;
}
