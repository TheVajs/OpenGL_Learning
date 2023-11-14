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

double last_x_pos, last_y_pos;
bool _first = true;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window, double deltatime);
void MouseCallback(GLFWwindow* window, double x_pos, double y_pos);
void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset);
void drawPointLights(Simp::Shader& shader, const Simp::World& world, GLuint vao, GLuint size);

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

	Simp::Shader phongShader;
	Simp::Shader whiteShader;
	phongShader.attach("phong.vert").attach("phong.frag").link();
	whiteShader.attach("white.vert").attach("white.frag").link();
	GLuint planeDiffuse = Simp::loadTexture(PROJECT_SOURCE_DIR "/Resources/wood.png");;

	Simp::World world(camera);
	Simp::DirectionalLight dirLight(glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)), glm::vec3(0.01f));
	world.attachLight(dirLight);
	Simp::OtherLight pointLight(glm::vec4(1.2f, 0.5f, 1.5f, 1.0f / 10.0f), glm::vec3(1.0f));
	world.attachLight(pointLight);
	Simp::OtherLight spotLight(glm::vec4(0.0f, 0.5f, 5.0f, 1.0f / 10.0f), glm::vec3(1.0f),
		glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)), 45.0f, 30.0f);
	world.attachLight(spotLight);
	world.bindBuffer(phongShader);
	world.bindLights();

	Simp::Model backpack(PROJECT_SOURCE_DIR "/Resources/backpack/backpack.obj");
	auto plane_vao = Simp::createPlane();
	auto light_cube_vao = Simp::createCube();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// glDepthMask(GL_FALSE);  disable or enbale writing to depth buffer

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// You need to sort by ascending depth order to render transparent objects,
	// So the depth test does not interfire with rendering. Also sort opeque bo ascending depth,
	// improves performance. But sorting allot of objects with different shapes is a hard task. 

	float currentframe = 0.0f;
	float previousframe = 0.0f;
	float deltatime;

	while (!glfwWindowShouldClose(window))
	{
		{
			currentframe = static_cast<float>(glfwGetTime());
			deltatime = currentframe - previousframe;
			previousframe = currentframe;
			ProcessInput(window, deltatime);
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawPointLights(whiteShader, world, light_cube_vao, 36);

		auto projection = camera.getProjectionMatrix();
		auto view = camera.getViewMatrix();

		phongShader.use();
		phongShader.bind("cameraPos", camera.getPosition());

		glm::mat4 modelBackpack(1.0f);
		modelBackpack = glm::translate(modelBackpack, glm::vec3(0.0f, 1.0f, 0.0f));
		modelBackpack = glm::scale(modelBackpack, glm::vec3(0.5f, 0.5f, 0.5f));
		phongShader.bind("model", modelBackpack);
		phongShader.bind("view", view);
		phongShader.bind("projection", projection);
		phongShader.bind("invModel", glm::mat3(glm::inverseTranspose(modelBackpack)));
		phongShader.bind("material.shininess", 32.0f);
		phongShader.bind("material.light_maps", Simp::Shader::DIFFUSE | Simp::Shader::SPECULAR);
		phongShader.bind("exposure", 2.0f);
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
		phongShader.bind("material.light_maps", Simp::Shader::DIFFUSE);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		phongShader.bind("material.texture_diffuse0", 0);
		phongShader.bind("material.specular", glm::vec3(1.0f));
		phongShader.bind("material.shininess", 16.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeDiffuse);
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

void drawPointLights(Simp::Shader& shader, const Simp::World& world, GLuint vao, GLuint size)
{
	auto lights = world.getOtherLights();
	for (unsigned int i = 0; i < lights.size(); i++)
	{
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(lights[i].pos));
		model = glm::scale(model, glm::vec3(0.2f));
		shader.use();
		shader.bind("view", camera.getViewMatrix());
		shader.bind("projection", camera.getProjectionMatrix());
		shader.bind("model", model);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, size);
		glBindVertexArray(0);
	}
}