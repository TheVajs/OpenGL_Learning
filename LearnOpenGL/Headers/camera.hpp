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
		Camera(glm::vec3 position, glm::vec3 yup, int width, int height);

		glm::vec3 getPosition() const { return position; }
		glm::mat4 getViewMatrix() const
		{
			return glm::lookAt(position, position + w, yup);
		}
		glm::mat4 getProjectionMatrix() const
		{
			return glm::perspective(glm::radians(zoom), aspectratio, zNear, zFar);
		}

		void resize(int _width, int _height);
		void processKeyboard(const glm::vec3& direction, float deltaTime);
		void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
		void processMouseScroll(float yoffset);

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

		void updateLocalVectors();
	};
}