#include "learnOpenGL.hpp"

// System headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "models.hpp"
#include "debug.hpp"

// Standard headers
#include <cstdio>
#include <stdio.h>
#include <iostream>

glm::vec3 cam_pos(0.0f, 0.0f, 5.0f);
glm::vec3 yup(0.0f, 1.0f, 0.0f);
Simp::Camera camera(cam_pos, yup, window_width, window_height);

double last_x_pos, last_y_pos;
bool _first = true;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window, double deltatime);
void MouseCallback(GLFWwindow* window, double x_pos, double y_pos);
void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset);

int main()
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

	stbi_set_flip_vertically_on_load(true);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		std::cout << "Failed to initialize 	GLAD." << std::endl;
		return -1;
	}
	// fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));
	Simp::debug();

	Simp::Shader phongShader;
	phongShader.attach("lighting.vert").attach("lighting.frag").link();
	Simp::Shader whiteShader;
	whiteShader.attach("white.vert").attach("white.frag").link();

	Simp::Model backpack(PROJECT_SOURCE_DIR "/Resources/backpack/backpack.obj");
	auto plane_vao = Simp::createPlane();
	auto light_cube_vao = Simp::createCube();

	float currentframe = 0.0f;
	float previousframe = 0.0f;
	float deltatime;

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		currentframe = static_cast<float>(glfwGetTime());
		deltatime = currentframe - previousframe;
		previousframe = currentframe;

		ProcessInput(window, deltatime);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		auto projection = camera.getProjectionMatrix();
		auto view = camera.getViewMatrix();

		glm::vec3 light_pos(1.2f, 0.5f, 1.5f);
		glm::vec3 light_color(.8f, .8f, .8f);

		whiteShader.use();
		whiteShader.bind("view", view);
		whiteShader.bind("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, light_pos);
		model = glm::scale(model, glm::vec3(0.2f));
		whiteShader.bind("model", model);

		glBindVertexArray(light_cube_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glm::mat4 model_backpack = glm::mat4(1.0f);
		model_backpack = glm::translate(model_backpack, glm::vec3(0.0f, 1.0f, 0.0f));
		model_backpack = glm::scale(model_backpack, glm::vec3(0.5f, 0.5f, 0.5f));
		glm::mat3 invModel = glm::mat3(glm::inverseTranspose(model_backpack));

		phongShader.use();
		// phongShader.bind("uTime", currentframe);de2wqd
		phongShader.bind("model", model_backpack);
		phongShader.bind("view", view);
		phongShader.bind("projection", projection);
		phongShader.bind("normal_model", invModel);
		phongShader.bind("material.light_maps", true);
		phongShader.bind("material.shininess", 32.0f);
		phongShader.bind("light.pos", light_pos);
		phongShader.bind("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		phongShader.bind("light.diffuse", light_color);
		phongShader.bind("light.specular", glm::vec3(1.0f));
		phongShader.bind("uCameraPos", camera.position);
		phongShader.bind("light.constant", 1.0f);
		phongShader.bind("light.linear", 0.09f);
		phongShader.bind("light.quadratic", 0.032f);
		backpack.draw(phongShader);

		glm::mat4 model2(1.0f);
		model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 2.0f));
		model2 = glm::scale(model2, glm::vec3(0.5f, 0.5f, 0.5f));
		phongShader.bind("model", model2);
		backpack.draw(phongShader);
			
		glm::mat4 model3(1.0f);
		model3 = glm::translate(model3, glm::vec3(0.0f, -1.0f, 0.0f));
		model3 = glm::scale(model3, glm::vec3(10.0f, 0.0f, 10.0f));
		phongShader.bind("model", model3);
		glBindVertexArray(plane_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &plane_vao);
	glDeleteVertexArrays(1, &light_cube_vao);

	glfwTerminate();
	return EXIT_SUCCESS;
}

void FramebufferSizeCallback(GLFWwindow*, int width, int height)
{
	glViewport(0, 0, width, height);
	camera.resize(width, height);
}

void ProcessInput(GLFWwindow* window, double deltatime)
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

	camera.processKeyboard(glm::normalize(direction), static_cast<float>(deltatime));
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