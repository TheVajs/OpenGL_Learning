#include "camera.hpp"

namespace Simp
{
	Camera::Camera(glm::vec3 position, glm::vec3 yup, int width, int height) : yaw(Camera::YAW), pitch(Camera::PITCH),
		movementSpeed(Camera::SPEED), mouseSensitivity(Camera::SENSITIVITY), verticalFov(Camera::FOV), zNear(0.1f), zFar(100.0f)
	{
		this->position = position;
		this->yup = yup;
		resize(width, height);
		updateLocalVectors();
	}

	glm::mat4 Camera::getViewMatrix() const
	{
		return glm::lookAt(position, position + w, yup);
	}

	glm::mat4 Camera::getProjectionMatrix() const
	{
		return glm::perspective(glm::radians(verticalFov), aspectratio, zNear, zFar);
	}

	void Camera::resize(int _width, int _height)
	{
		width = _width;
		height = _height;
		aspectratio = static_cast<float>(width) / static_cast<float>(height);
	}

	void Camera::processKeyboard(const glm::vec3& dir, float deltaTime)
	{
		glm::mat3 invRot = glm::transpose(glm::mat3(getViewMatrix()));
		position += invRot * -dir * (movementSpeed * deltaTime);
	}

	void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
	{
		xoffset *= mouseSensitivity;
		yoffset *= mouseSensitivity;

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
		verticalFov -= yoffset;
		if (verticalFov < 1.0f)
			verticalFov = 1.0f;
		if (verticalFov > 45.0f)
			verticalFov = 45.0f;
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