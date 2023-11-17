#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace Simp
{
	class Camera
	{
		static constexpr float YAW = -90.0f;
		static constexpr float PITCH = 0.0f;
		static constexpr float SPEED = 5.0f;
		static constexpr float SENSITIVITY = 0.075f;
		static constexpr float FOV = 45.0f;

	public:
		Camera(glm::vec3 position, glm::vec3 yup, int width, int height);

		glm::vec3 getPosition() const { return position; }
		glm::mat4 getViewMatrix() const;
		glm::mat4 getProjectionMatrix() const;

		void resize(int _width, int _height);
		void processKeyboard(const glm::vec3& dir, float deltaTime);
		void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
		void processMouseScroll(float yoffset);

	private:
		glm::vec3 position;
		glm::vec3 w, u, v;
		glm::vec3 yup;

		int width;
		int height;
		float aspectratio;
		float zNear;
		float zFar;

		float yaw, pitch;
		float movementSpeed;
		float mouseSensitivity;
		float verticalFov;

		void updateLocalVectors();
	};
}