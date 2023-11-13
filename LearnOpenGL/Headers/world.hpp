#pragma once

#include <glm/glm.hpp>

#include "shader.hpp"
#include "camera.hpp"

#include <vector>

namespace Simp
{
	const unsigned int MAX_DIR_LIGHTS = 4;
	const unsigned int MAX_OTHER_LIGTHS = 16;

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
		float inner, outer;
	};

	class World
	{
	public:
		World(const Camera& cam) : camera(cam) {}

		World& attachLight(const DirectionalLight& light);
		World& attachLight(const OtherLight& light);

		void bind(Shader& shader);

	private:
		unsigned int directionalLightNum;
		unsigned int otherLightNum;

		std::vector<DirectionalLight> directionalLights;
		std::vector<OtherLight> otherLights;

		const Camera camera;
	};
}