#include "learnOpenGL.hpp"
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "models.hpp"
#include "world.hpp"
#include "debug.hpp"

// Standard headers
#include <cstdio>
#include <stdio.h>
#include <iostream>

glm::vec3 cam_pos(0.0f, 0.0f, 5.0f);
glm::vec3 yup(0.0f, 1.0f, 0.0f);
Simp::Camera camera(cam_pos, yup, window_width, window_height);

double lastPosX, lastPosY;
bool _first = true;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window, double deltaTime);
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
	Simp::debug();

	GLuint planeDiffuse = Simp::loadTexture(PROJECT_SOURCE_DIR "/Resources/wood.png");

	Simp::World world;

	// auto dirLight{ std::make_unique<Simp::DirectionalLight>(
	// 	glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)),
	// 	glm::vec3(0.8f))
	// };
	// world.attachLight(dirLight);

	auto pointLight{ std::make_unique<Simp::OtherLight>(glm::vec4(1.2f, 0.5f, 1.5f, 1.0f / 10.0f), glm::vec3(1.0f)) };
	world.attachLight(pointLight);
	auto spotLight{ std::make_unique<Simp::OtherLight>(glm::vec4(0.0f, 0.5f, 5.0f, 1.0f / 50.0f), glm::vec3(20.0f),
		glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)), 30.0f, 25.0f) };
	world.attachLight(spotLight);

	// Shaders

	Simp::Shader phongShader;
	phongShader.attach("phong.vert").attach("phong.frag").link();
	world.bindBuffer(phongShader);
	Simp::Shader whiteShader;
	whiteShader.attach("white.vert").attach("white.frag").link();
	Simp::Shader screenShader;
	screenShader.attach("screen.vert").attach("screen.frag").link();

	// Models

	Simp::Model backpack(PROJECT_SOURCE_DIR "/Resources/backpack/backpack.obj");
	GLuint plane_vao = Simp::createPlane();
	GLuint light_cube_vao = Simp::createCube();

	// Framebuffers
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // GL_READ_FRAMEBUFFER, GL_DRAW_FRAMEBUFFER

	GLuint tbo;
	glGenTextures(1, &tbo);
	glBindTexture(GL_TEXTURE_2D, tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, window_width, window_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbo, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR:: framebuffer not complete!" << std::endl;
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// You need to sort by ascending depth order to render transparent objects,
	// So the depth test does not interfire with rendering. Also sort opeque bo ascending depth,
	// improves performance. But sorting allot of objects with different shapes is a hard task. 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float current = 0.0f;
	float previous = 0.0f;
	float time = 0.0f;
	float deltaTime;

	while (!glfwWindowShouldClose(window))
	{
		{
			current = static_cast<float>(glfwGetTime());
			deltaTime = current - previous;
			time += deltaTime;
			previous = current;
			ProcessInput(window, deltaTime);
		}

		auto point{ world.getOtherLights()[0].get() };
		point->pos.x = 2.0f * glm::cos(.25f * glm::pi<float>() * time);
		point->pos.z = 2.0f * glm::sin(.25f * glm::pi<float>() * time);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		world.bindLights();
		world.drawPointLights(camera, whiteShader, light_cube_vao, 36);

		phongShader.use();
		phongShader.bind("cameraPos", camera.getPosition());
		glm::mat4 modelBackpack(1.0f);
		modelBackpack = glm::translate(modelBackpack, glm::vec3(0.0f, 1.0f, 0.0f));
		modelBackpack = glm::scale(modelBackpack, glm::vec3(0.5f, 0.5f, 0.5f));
		phongShader.bind("model", modelBackpack);
		phongShader.bind("view", camera.getViewMatrix());
		phongShader.bind("projection", camera.getProjectionMatrix());
		phongShader.bind("invModel", glm::mat3(glm::inverseTranspose(modelBackpack)));
		phongShader.bind("material.shininess", 32.0f);
		phongShader.bind("material.maps", Simp::Shader::DIFFUSE | Simp::Shader::SPECULAR);
		phongShader.bind("exposure", 1.0f);
		backpack.draw(phongShader);

		glm::mat4 model3(1.0f);
		model3 = glm::translate(model3, glm::vec3(0.0f, -1.0f, 0.0f));
		model3 = glm::scale(model3, glm::vec3(10.0f, 0.0f, 10.0f));
		phongShader.bind("model", model3);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		phongShader.bind("material.texture_diffuse0", 0);
		phongShader.bind("material.maps", Simp::Shader::DIFFUSE);
		phongShader.bind("material.specular", glm::vec3(1.0f));
		phongShader.bind("material.shininess", 16.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeDiffuse);
		glBindVertexArray(plane_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		screenShader.use();
		glBindTexture(GL_TEXTURE_2D, tbo);
		screenShader.bind("screenTexture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	GLuint arr[3] { plane_vao, light_cube_vao };
	glDeleteVertexArrays(3, arr);

	glDeleteTextures(1, &tbo);
	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &fbo);

	glfwTerminate();
	return EXIT_SUCCESS;
}

void FramebufferSizeCallback(GLFWwindow*, int width, int height)
{
	glViewport(0, 0, width, height);
	camera.resize(width, height);
}

void ProcessInput(GLFWwindow* window, double deltaTime)
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

	camera.processKeyboard(glm::normalize(direction), static_cast<float>(deltaTime));
}

void MouseCallback(GLFWwindow*, double x_pos, double y_pos)
{
	if (_first)
	{
		lastPosX = x_pos;
		lastPosY = y_pos;
		_first = false;
	}

	double offsetX = x_pos - lastPosX;
	double offsetY = lastPosY - y_pos;
	lastPosX = x_pos;
	lastPosY = y_pos;
	camera.processMouseMovement(static_cast<float>(offsetX), static_cast<float>(offsetY));
}

void ScrollCallback(GLFWwindow*, double, double y_offset)
{
	camera.processMouseScroll(static_cast<float>(y_offset));
}