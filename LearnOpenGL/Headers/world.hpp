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
		DirectionalLight(glm::vec3 _dir, glm::vec3 _color) : dir(_dir), color(_color) {}
	};

	struct OtherLight
	{
		glm::vec4 pos;
		glm::vec3 color;
		glm::vec3 dir;
		glm::vec2 angles;

		OtherLight(glm::vec4 _pos, glm::vec3 _color)
			: pos(_pos), color(_color), dir(glm::vec3(1.0f, 0.0f, 0.0f))
		{
			angles = calculateSpotAngle(360.0f, 360.0f);
		}

		OtherLight(glm::vec4 _pos, glm::vec3 _color, glm::vec3 _dir, float outter, float inner)
			: pos(_pos), color(_color), dir(_dir)
		{
			angles = calculateSpotAngle(outter, inner);
		}

		glm::vec2 calculateSpotAngle(float outter, float inner) const
		{
			float outCos = glm::cos(glm::radians(outter) * 0.5f);
			float inCos = glm::cos(glm::radians(inner) * 0.5f);
			float invRange = 1.0f / fmax(inCos - outCos, 1e-4f);
			return glm::vec2(invRange, -outCos * invRange);
		}
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
		// TODO bind camera to shader
		void bindLights();

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