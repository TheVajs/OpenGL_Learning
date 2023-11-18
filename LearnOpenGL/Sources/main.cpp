#include "learnOpenGL.hpp"

#include <iostream> // vs cstdio/stdio.h

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "texture.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "models.hpp"
#include "world.hpp"
#include "debug.hpp"

glm::f64vec2 lastMousePos;
glm::vec3 camPos(0.0f, 1.0f, 5.0f);
Simp::Camera camera(camPos, glm::vec3(0.0f, 1.0f, 0.0f), cWindowWidth, cWindowHeight);

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window, double deltaTime);
void MouseCallback(GLFWwindow* window, double x_pos, double y_pos);
void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset);

GLuint* initializeFrameBuffer()
{
	GLuint fbo;
	GLuint tbo;
	GLuint rbo;
	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &tbo);
	glGenRenderbuffers(1, &rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // GL_READ_FRAMEBUFFER, GL_DRAW_FRAMEBUFFER
	glBindTexture(GL_TEXTURE_2D, tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cWindowWidth, cWindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, cWindowWidth, cWindowHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbo, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "ERROR:: framebuffer not complete!" << std::endl;
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	static GLuint arr[3]{ fbo, tbo, rbo };
	return arr;
}

void deleteFrameBuffer(GLuint* bh)
{
	glDeleteTextures(1, &bh[0]);
	glDeleteRenderbuffers(1, &bh[1]);
	glDeleteFramebuffers(1, (bh + 2));
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	auto window = glfwCreateWindow(cWindowWidth, cWindowHeight, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize 	GLAD." << std::endl;
		return -1;
	}
	Simp::debug();
	lastMousePos.x = cWindowWidth * .5;
	lastMousePos.y = cWindowHeight * .5;

	Simp::World world;
	// auto dirLight{ std::make_unique<Simp::DirectionalLight>(
	// 	glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)), glm::vec3(0.91f)) };
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
	Simp::Shader skyboxShader;
	skyboxShader.attach("sky/sky.vert").attach("sky/sky.frag").link();

	// Models & Textures

	Simp::Model backpack(PROJECT_SOURCE_DIR "/Resources/meshes/backpack/backpack.obj");
	GLuint vaoPlane = Simp::createPlane();
	GLuint vaoCube = Simp::createCube();
	GLuint textureDiffuseWood = Simp::loadTexture(PROJECT_SOURCE_DIR "/Resources/Textures/wood.png", false);

	std::vector<std::string> cubeFaces{
		PROJECT_SOURCE_DIR "/Resources/Textures/skybox/right.jpg",
		PROJECT_SOURCE_DIR "/Resources/Textures/skybox/left.jpg",
		PROJECT_SOURCE_DIR "/Resources/Textures/skybox/top.jpg",
		PROJECT_SOURCE_DIR "/Resources/Textures/skybox/bottom.jpg",
		PROJECT_SOURCE_DIR "/Resources/Textures/skybox/front.jpg",
		PROJECT_SOURCE_DIR "/Resources/Textures/skybox/back.jpg"
	};
	GLuint textureCubeMap = Simp::loadCubemap(cubeFaces, false);

	// Frame buffer / Texture buffer / Render buffer

	GLuint* bufferHandels = initializeFrameBuffer();

	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

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

		// Update objects

		auto& point { *world.getOtherLights()[0].get() };
		point.pos.x = 2.0f * glm::cos(.25f * glm::pi<float>() * time);
		point.pos.z = 2.0f * glm::sin(.25f * glm::pi<float>() * time);

		// Pass 1

		glBindFramebuffer(GL_FRAMEBUFFER, bufferHandels[0]);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		world.bindLights();
		world.drawPointLights(camera, whiteShader, vaoCube, 36);

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
		phongShader.bind("material.texture_diffuse0", 0);
		phongShader.bind("material.maps", Simp::Shader::DIFFUSE);
		phongShader.bind("material.specular", glm::vec3(1.0f));
		phongShader.bind("material.shininess", 64.0f);
		glBindTexture(GL_TEXTURE_2D, textureDiffuseWood);
		glBindVertexArray(vaoPlane);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Draw sky box last

		glDisable(GL_CULL_FACE);
		skyboxShader.use();
		skyboxShader.bind("skybox", 0);
		skyboxShader.bind("view", glm::mat4(glm::mat3(camera.getViewMatrix())));
		skyboxShader.bind("projection", camera.getProjectionMatrix());
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureCubeMap);
		glBindVertexArray(vaoCube);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glEnable(GL_CULL_FACE);

		// Pass 2

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		screenShader.use();
		glBindTexture(GL_TEXTURE_2D, bufferHandels[1]);
		screenShader.bind("screenTexture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
		glFinish();
	}

	GLuint arr[3]{ vaoPlane, vaoCube };
	glDeleteVertexArrays(3, arr);
	deleteFrameBuffer(bufferHandels);
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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);

	glm::vec3 dir = glm::vec3(0.0f);
	if (glfwGetKey(window, GLFW_KEY_W))
		dir += glm::vec3(0.0f, 0.0f, 1.0f);
	if (glfwGetKey(window, GLFW_KEY_S))
		dir += -glm::vec3(0.0f, 0.0f, 1.0f);
	if (glfwGetKey(window, GLFW_KEY_D))
		dir += -glm::vec3(1.0f, 0.0f, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_A))
		dir += glm::vec3(1.0f, 0.0f, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_C))
		dir += glm::vec3(0.0f, 1.0f, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_SPACE))
		dir += glm::vec3(0.0f, -1.0f, 0.0f);

	if (glm::length(dir) > 1e-1f)
		camera.processKeyboard(glm::normalize(dir), static_cast<float>(deltaTime));
}

void MouseCallback(GLFWwindow*, double x_pos, double y_pos)
{
	auto offsetX = x_pos - lastMousePos.x;
	auto offsetY = lastMousePos.y - y_pos;
	camera.processMouseMovement(static_cast<float>(offsetX), static_cast<float>(offsetY));
	lastMousePos = glm::f64vec2(x_pos, y_pos);
}

void ScrollCallback(GLFWwindow*, double, double y_offset)
{
	camera.processMouseScroll(static_cast<float>(y_offset));
}