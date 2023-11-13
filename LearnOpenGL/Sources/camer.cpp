#include "camera.hpp"

namespace Simp
{
	Camera::Camera(glm::vec3 position, glm::vec3 yup, int width, int height) : yaw(YAW), pitch(PITCH),
		movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM), zNear(0.1f), zFar(100.0f)
	{
		this->position = position;
		this->yup = yup;
		resize(width, height);
		updateLocalVectors();
	}

	void Camera::resize(int _width, int _height)
	{
		width = _width;
		height = _height;
		aspectratio = static_cast<float>(width) / static_cast<float>(height);
	}

	void Camera::processKeyboard(const glm::vec3& direction, float deltaTime)
	{
		glm::mat3 invRot = glm::inverse(glm::mat3(getViewMatrix()));
		position += invRot * -direction * (movement_speed * deltaTime);
	}

	void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
	{
		xoffset *= mouse_sensitivity;
		yoffset *= mouse_sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (constrainPitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		updateLocalVectors();
	}

	void Camera::processMouseScroll(float yoffset)
	{
		zoom -= yoffset;
		if (zoom < 1.0f)
			zoom = 1.0f;
		if (zoom > 45.0f)
			zoom = 45.0f;
	}

	void Camera::updateLocalVectors()
	{
		glm::vec3 forward;
		forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward.y = sin(glm::radians(pitch));
		forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		w = glm::normalize(forward);
		u = glm::normalize(glm::cross(w, yup));
		v = glm::normalize(glm::cross(u, w));
	}
}