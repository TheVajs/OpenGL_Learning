#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "camera.hpp"

#include <vector>
#include <cassert>

namespace Simp
{
	struct DirectionalLight
	{
		glm::vec3 dir;
		glm::vec3 color;
	};

	struct OtherLight
	{
		glm::vec4 pos;
		glm::vec3 color;
		glm::vec3 dir;
		glm::vec2 angles;
	};

	class World
	{
	public:
		const static unsigned int MAX_DIRECTIONAL_LIGHTS = 4;
		const static unsigned int MAX_OTHER_LIGHTS = 32;

		static constexpr const char* uboName = "Lights";
		const static unsigned int uboSize =
			sizeof(GLint) * 4 + MAX_DIRECTIONAL_LIGHTS * (sizeof(glm::vec4) * 2) +
			MAX_OTHER_LIGHTS * (sizeof(glm::vec4) * 4);

		World(const Camera& cam);
		~World() { glDeleteBuffers(1, &ubo); }

		World& attachLight(const DirectionalLight& light);
		World& attachLight(const OtherLight& light);

		void bindBuffer(const Shader& shader);
		void bind();

		const std::vector<DirectionalLight>& getDirectionalLights() const
		{
			return directionalLights;
		}

		const std::vector<OtherLight>& getOtherLights() const
		{
			return otherLights;
		}

	private:
		GLuint ubo;
		std::vector<DirectionalLight> directionalLights;
		std::vector<OtherLight> otherLights;
		const Camera camera;

		// Disable Copying and Assignment
		World(World const&) = delete;
		World& operator=(World const&) = delete;
	};
}