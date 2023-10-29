#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.075f;
const float ZOOM = 45.0f;


class Camera
{
public:
	glm::vec3 position;
	glm::vec3 w, u, v;
	glm::vec3 yup;

	int width;
	int height;
	float aspect_ratio;
	float near;
	float far;

	float yaw, pitch;
	float movement_speed;
	float mouse_sensitivity;
	float zoom;

	Camera(glm::vec3 _position, glm::vec3 _yup, int _width, int _height) : w(glm::vec3(0.0f, 0.0f, -1.0f)), yaw(YAW), pitch(PITCH),
		movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM), near(0.1f), far(100.0f)
	{
		position = _position;
		yup = _yup;
		resize(_width, _height);
		updateVectors();
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(position, position + w, yup);
	}

	glm::mat4 getProjectionMatrix()
	{
		return glm::perspective(glm::radians(zoom), aspect_ratio, near, far);
	}

	void resize(int _width, int _height) 
	{
		width = _width;
		height = _height;
		aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
	}

	void processKeyboard(const glm::vec3& direction, float delta_time)
	{
		float velocity = movement_speed * delta_time;
		position += glm::inverse(glm::mat3(getViewMatrix())) * -direction * velocity;
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

		updateVectors();
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
	void updateVectors()
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

#endif