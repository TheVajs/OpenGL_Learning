#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_XYZW_ONLY
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"
#include "camera.h"
#include "shapes.h"

int window_width = 1200;
int window_height = 900;
glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 yup = glm::vec3(0.0f, 1.0f, 0.0f);

Camera camera = Camera(cam_pos, yup, window_width, window_height);

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float delta_time);
void mouseCallback(GLFWwindow* window, double x_pos, double y_pos);
void scrollCallback(GLFWwindow* window, double x_offset, double y_offset);

unsigned int loadImage(char const* texture_path)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(texture_path, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "WARNING::Failed to load image!" << std::endl;
	}
	stbi_image_free(data);

	return texture;
}

int main()
{
	GLFWwindow* window;
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(window_width, window_height, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);

	glBindVertexArray(cube_vao);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int light_cube_vao;
    glGenVertexArrays(1, &light_cube_vao);
    glBindVertexArray(light_cube_vao);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	unsigned int texture = loadImage("resources\\container.jpg");
	Shader lighting_shader("shaders\\lighting.vert", "shaders\\lighting.frag");
	Shader white_shader("shaders\\white.vert", "shaders\\white.frag");

	float current_frame, previous_frame;
	float delta_time;

	glViewport(0, 0, window_width, window_height);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		current_frame = glfwGetTime();
		delta_time = current_frame - previous_frame;

		processInput(window, delta_time);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto projection = camera.getProjectionMatrix();
		auto view = camera.getViewMatrix();

		glm::vec3 light_pos(1.2f, 1.0f, 1.5f);
		glm::vec3 light_color(1.0f, 1.0f, 1.0f);

		white_shader.use();
		white_shader.setFloat("uTime", current_frame);
		white_shader.setMat4("view", view);
		white_shader.setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, light_pos);
        model = glm::scale(model, glm::vec3(0.2f));
		white_shader.setMat4("model", model);

		glBindVertexArray(light_cube_vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		model = glm::mat4(1.0f);
		glm::mat4 invModel = glm::inverseTranspose(model);

		lighting_shader.use();
		lighting_shader.setFloat("uTime", current_frame);
		lighting_shader.setMat4("model", model);
		lighting_shader.setMat4("invModel", invModel);
		lighting_shader.setMat4("view", view);
		lighting_shader.setMat4("projection", projection);

		lighting_shader.setVec3("uObjectColor", glm::vec3(1.0f, 0.5f, 0.31f));
		lighting_shader.setVec3("uAmbientLight", glm::vec3(0.1f, 0.1f, 0.1f));
		lighting_shader.setVec3("uLightColor", light_color);
		lighting_shader.setVec3("uLightPos", light_pos);
		lighting_shader.setVec3("uCameraPos", camera.position);

		glBindVertexArray(cube_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// // // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		previous_frame = current_frame;
	}

	glfwTerminate();
	return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	camera.resize(width, height);
}

void processInput(GLFWwindow* window, float delta_time)
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

	if (glm::length(direction) > 0.0f)
	{
		camera.processKeyboard(glm::normalize(direction), delta_time);
	}
}

float last_x_pos, last_y_pos;
bool first = true;

void mouseCallback(GLFWwindow* window, double x_pos, double y_pos)
{
	if (first)
	{
		last_x_pos = x_pos, last_y_pos = y_pos;
		first = false;
	}

	float x_offset = x_pos - last_x_pos;
	float y_offset = last_y_pos - y_pos;
	last_x_pos = x_pos;
	last_y_pos = y_pos;

	camera.processMouseMovement(x_offset, y_offset);
}

void scrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
	camera.processMouseScroll(y_offset);
}