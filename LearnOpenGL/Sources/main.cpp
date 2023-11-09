#include "learnOpenGL.hpp"

// System headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "models.hpp"

// Standard headers
#include <cstdio>
#include <stdio.h>
#include <iostream>

glm::vec3 cam_pos(0.0f, 0.0f, 5.0f);
glm::vec3 yup(0.0f, 1.0f, 0.0f);
Simp::Camera camera(cam_pos, yup, window_width, window_height);

double last_x_pos, last_y_pos;
bool _first = true;

GLFWwindow* InitializeGLFWwindow();
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window, double delta_time);
void MouseCallback(GLFWwindow* window, double x_pos, double y_pos);
void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset);

int main()
{
	GLFWwindow* window = InitializeGLFWwindow();

	stbi_set_flip_vertically_on_load(true);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize 	GLAD. Manages OpenGL functions!" << std::endl;
		return -1;
	}

	unsigned int cube_vbo, cube_vao;
	glGenVertexArrays(1, &cube_vao);
	glGenBuffers(1, &cube_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_all), cube_all, GL_STATIC_DRAW);

	glBindVertexArray(cube_vao);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int light_cube_vao;
	glGenVertexArrays(1, &light_cube_vao);
	glBindVertexArray(light_cube_vao);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int plane_vao, plane_vbo;
	glGenVertexArrays(1, &plane_vao);
	glGenBuffers(1, &plane_vbo);
	glBindVertexArray(plane_vao);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_plane), vertices_plane, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	GLuint diffuse_map = Simp::loadTexture(PROJECT_SOURCE_DIR "/Resources/container2.png");
	GLuint specular_map = Simp::loadTexture(PROJECT_SOURCE_DIR "/Resources/container2_specular.png");

	Simp::Model backpack = Simp::Model(PROJECT_SOURCE_DIR "/Resources/backpack/backpack.obj");

	Simp::Shader lighting_shader("lighting.vert", "lighting.frag");
	lighting_shader.link();
	Simp::Shader white_shader("white.vert", "white.frag");
	white_shader.link();

	double current_frame = 0.0;
	double previous_frame = 0.0;
	double delta_time;

	glViewport(0, 0, window_width, window_height);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		current_frame = glfwGetTime();
		delta_time = current_frame - previous_frame;

		ProcessInput(window, static_cast<float>(delta_time));

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto projection = camera.getProjectionMatrix();
		auto view = camera.getViewMatrix();

		glm::vec3 light_pos(1.2f, 0.5f, 1.5f);
		glm::vec3 light_color(.5f, .5f, .5f);

		white_shader.use();
		white_shader.setFloat("uTime", static_cast<float>(current_frame));
		white_shader.setMat4("view", view);
		white_shader.setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, light_pos);
		model = glm::scale(model, glm::vec3(0.2f));
		white_shader.setMat4("model", model);

		glBindVertexArray(light_cube_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4(1.0f);
		glm::mat3 invModel = glm::mat3(glm::inverseTranspose(model));

		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, diffuse_map);
		// glActiveTexture(GL_TEXTURE1);
		// glBindTexture(GL_TEXTURE_2D, specular_map);

		// lighting_shader.use();
		// lighting_shader.setFloat("uTime", static_cast<float>(current_frame));
		// lighting_shader.setMat4("model", model);
		// lighting_shader.setMat4("view", view);
		// lighting_shader.setMat4("projection", projection);
		// lighting_shader.setMat3("normal_model", invModel);

		// lighting_shader.setBool("material.light_maps", true);
		// lighting_shader.setInt("material.texture_diffuse1", 0);
		// lighting_shader.setInt("material.texture_specular1", 1);
		// lighting_shader.setFloat("material.shininess", 64.0f);
		// lighting_shader.setVec3("light.pos", light_pos);
		// lighting_shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		// lighting_shader.setVec3("light.diffuse", light_color);
		// lighting_shader.setVec3("light.specular", glm::vec3(1.0f));
		// lighting_shader.setVec3("uCameraPos", camera.position);
		// lighting_shader.setFloat("light.constant", 1.0f);
		// lighting_shader.setFloat("light.linear", 0.09f);
		// lighting_shader.setFloat("light.quadratic", 0.032f);

		// glBindVertexArray(cube_vao);
		// glDrawArrays(GL_TRIANGLES, 0, 36);

		lighting_shader.use();
		lighting_shader.setFloat("uTime", static_cast<float>(current_frame));
		lighting_shader.setMat4("model", model);
		lighting_shader.setMat4("view", view);
		lighting_shader.setMat4("projection", projection);
		lighting_shader.setMat3("normal_model", invModel);
		lighting_shader.setBool("material.light_maps", true);
		lighting_shader.setFloat("material.shininess", 64.0f);
		lighting_shader.setVec3("light.pos", light_pos);
		lighting_shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		lighting_shader.setVec3("light.diffuse", light_color);
		lighting_shader.setVec3("light.specular", glm::vec3(1.0f));
		lighting_shader.setVec3("uCameraPos", camera.position);
		lighting_shader.setFloat("light.constant", 1.0f);
		lighting_shader.setFloat("light.linear", 0.09f);
		lighting_shader.setFloat("light.quadratic", 0.032f);
		
		backpack.draw(lighting_shader);

		lighting_shader.use();
		glm::mat4 modelFloor = glm::mat4(1.0f);
		modelFloor = glm::translate(modelFloor, glm::vec3(-0.5f));
		modelFloor = glm::scale(modelFloor, glm::vec3(10.0f));
		invModel = glm::mat3(glm::inverseTranspose(modelFloor));
		lighting_shader.setMat4("model", modelFloor);
		lighting_shader.setMat4("view", view);
		lighting_shader.setMat4("projection", projection);
		lighting_shader.setMat3("normal_model", invModel);
		lighting_shader.setVec3("material.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		lighting_shader.setVec3("material.specular", glm::vec3(1.0f));
		lighting_shader.setFloat("material.shininess", 32.0f);
		lighting_shader.setVec3("light.pos", light_pos);
		lighting_shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		lighting_shader.setVec3("light.diffuse", light_color);
		lighting_shader.setVec3("light.specular", glm::vec3(1.0f));
		lighting_shader.setVec3("uCameraPos", camera.position);

		lighting_shader.setFloat("light.constant", 1.0f);
		lighting_shader.setFloat("light.linear", 0.09f);
		lighting_shader.setFloat("light.quadratic", 0.032f);

		glBindVertexArray(plane_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();

		previous_frame = current_frame;
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}

void FramebufferSizeCallback(GLFWwindow*, int width, int height)
{
	glViewport(0, 0, width, height);
	camera.resize(width, height);
}

void ProcessInput(GLFWwindow* window, double delta_time)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	glm::vec3 direction = glm::vec3(0.0f);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		direction += glm::vec3(0.0f, 0.0f, 1.0f);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		direction += -glm::vec3(0.0f, 0.0f, 1.0f);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		direction += -glm::vec3(1.0f, 0.0f, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		direction += glm::vec3(1.0f, 0.0f, 0.0f);

	if (glm::length(direction) <= 0.0f)
		return;

	camera.processKeyboard(glm::normalize(direction), static_cast<float>(delta_time));
}

void MouseCallback(GLFWwindow*, double x_pos, double y_pos)
{
	if (_first)
	{
		last_x_pos = x_pos, last_y_pos = y_pos;
		_first = false;
	}

	double x_offset = x_pos - last_x_pos;
	double y_offset = last_y_pos - y_pos;
	last_x_pos = x_pos;
	last_y_pos = y_pos;

	camera.processMouseMovement(static_cast<float>(x_offset), static_cast<float>(y_offset));
}

void ScrollCallback(GLFWwindow*, double, double y_offset)
{
	camera.processMouseScroll(static_cast<float>(y_offset));
}

GLFWwindow* InitializeGLFWwindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	auto window = glfwCreateWindow(window_width, window_height, "LearnOpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return window;
}