#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "shader.hpp"
#include "camera.hpp"

#ifndef SIMP_ASSERT
	#include <cassert>
	#define SIMP_ASSERT(c) assert(c)
#endif

namespace Simp
{
#pragma pack(push, 1)
	struct DirectionalLight
	{
		glm::vec3 dir;
		glm::vec3 color;

		DirectionalLight(glm::vec3 _dir, glm::vec3 _color) : dir(_dir), color(_color) {}
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct OtherLight
	{
		glm::vec4 pos;
		glm::vec3 color;
		glm::vec3 dir;
		glm::vec2 angles;

		OtherLight(glm::vec4 _pos, glm::vec3 _color);
		OtherLight(glm::vec4 _pos, glm::vec3 _color, glm::vec3 _dir, float outter, float inner);

	private:
		glm::vec2 calculateSpotAngle(float outter, float inner) const;
	};
#pragma pack(pop)

	class World
	{
	public:
		const static unsigned int MAX_DIRECTIONAL_LIGHTS = 4;
		const static unsigned int MAX_OTHER_LIGHTS = 32;
		static constexpr const char* uboName = "Lights";
		const static unsigned int uboSize = sizeof(GLint) * 4 +
			MAX_DIRECTIONAL_LIGHTS * (sizeof(glm::vec4) * 2) +
			MAX_OTHER_LIGHTS * (sizeof(glm::vec4) * 4);

		World();
		~World() { glDeleteBuffers(1, &ubo); }

		World& attachLight(std::unique_ptr<DirectionalLight>& light);
		World& attachLight(std::unique_ptr<OtherLight>& light);

		void bindBuffer(const Shader& shader);
		void bindLights();

		const std::vector<std::unique_ptr<DirectionalLight>>& getDirectionalLights() const;
		const std::vector<std::unique_ptr<OtherLight>>& getOtherLights() const;

		void drawPointLights(const Camera& camera, Shader& shader, GLuint vao, GLuint size) const;

	private:
		GLuint ubo;
		std::vector<std::unique_ptr<DirectionalLight>> directionalLights;
		std::vector<std::unique_ptr<OtherLight>> otherLights;
	};
}