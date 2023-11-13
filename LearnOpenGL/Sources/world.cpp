#include "world.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Simp
{
	World& World::attachLight(const DirectionalLight& light)
	{
		directionalLights.push_back(light);
		return *this;
	}

	World& World::attachLight(const OtherLight& light)
	{
		otherLights.push_back(light);
		return *this;
	}

	void World::bind(Shader& shader)
	{
		shader.bind("ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		shader.bind("cameraPos", camera.getPosition());
		shader.bind("directionalLightCount", 1);
		shader.bind("directionalLightDirections[0]", directionalLights[0].dir);
		shader.bind("directionalLightColor[0]", directionalLights[0].color);
		shader.bind("directionalLightCount", 0);
	}
}