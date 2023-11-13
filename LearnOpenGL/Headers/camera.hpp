#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

namespace Simp
{
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 5.0f;
	const float SENSITIVITY = 0.075f;
	const float ZOOM = 45.0f;

	class Camera
	{
	public:
		Camera(glm::vec3 position, glm::vec3 yup, int width, int height) : yaw(YAW), pitch(PITCH),
			movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM), zNear(0.1f), zFar(100.0f)
		{
			this->position = position;
			this->yup = yup;
			
			resize(width, height);

			updateLocalVectors();
		}

		glm::mat4 getViewMatrix() const
		{
			return glm::lookAt(position, position + w, yup);
		}

		glm::mat4 getProjectionMatrix() const
		{
			return glm::perspective(glm::radians(zoom), aspectratio, zNear, zFar);
		}

		glm::vec3 getPosition() const
		{
			return position;
		}

		void resize(int _width, int _height)
		{
			width = _width;
			height = _height;
			aspectratio = static_cast<float>(width) / static_cast<float>(height);
		}

		void processKeyboard(const glm::vec3& direction, float delta_time)
		{
			glm::mat3 invRot = glm::inverse(glm::mat3(getViewMatrix()));
			position += invRot * -direction * (movement_speed * delta_time);
		}

		void processMouseMovement(float xoffset, float yoffset, GLboolean constrain_pitch = true)
		{
			xoffset *= mouse_sensitivity;
			yoffset *= mouse_sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			if (constrain_pitch)
			{
				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;
			}

			updateLocalVectors();
		}

		void processMouseScroll(float yoffset)
		{
			zoom -= yoffset;
			if (zoom < 1.0f)
				zoom = 1.0f;
			if (zoom > 45.0f)
				zoom = 45.0f;
		}

	private:
		glm::vec3 position;
		glm::vec3 w, u, v;
		glm::vec3 yup;

		int width;
		int height;
		float aspectratio;
		float zNear, zFar;

		float yaw, pitch;
		float movement_speed;
		float mouse_sensitivity;
		float zoom;

		void updateLocalVectors()
		{
			glm::vec3 forward;
			forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			forward.y = sin(glm::radians(pitch));
			forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			w = glm::normalize(forward);
			u = glm::normalize(glm::cross(w, yup));
			v = glm::normalize(glm::cross(u, w));
		}
	};
}