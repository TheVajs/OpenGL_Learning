#include "world.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>

namespace Simp
{
	World::World(const Camera& cam) : camera(cam)
	{
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, uboSize, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	World& World::attachLight(const DirectionalLight& light)
	{
		assert(directionalLights.size() < MAX_DIRECTIONAL_LIGHTS);
		directionalLights.push_back(light);
		return *this;
	}

	World& World::attachLight(const OtherLight& light)
	{
		assert(otherLights.size() < MAX_OTHER_LIGHTS);
		otherLights.push_back(light);
		return *this;
	}

	void World::bindBuffer(const Shader& shader)
	{
		auto id = shader.getHandle();
		GLsizei uniformBlockSize;

		GLuint uniformBlockIndex = glGetUniformBlockIndex(id, uboName);
		glGetActiveUniformBlockiv(id, uniformBlockIndex,
									GL_UNIFORM_BLOCK_DATA_SIZE,
									&uniformBlockSize);

		// Attach the buffer to UBO binding point 0.
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
		// Associate the uniform block to this binding point.
		glUniformBlockBinding(id, uniformBlockIndex, 0);
	}

	void World::bind()
	{
		GLuint offset = 0;

		// layout 140 explenation
		// https://registry.khronos.org/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		auto size = static_cast<GLint>(directionalLights.size());
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &size);
		size = static_cast<GLint>(otherLights.size());
		glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &size);
		offset += 16;

		for (unsigned int i = 0; i < directionalLights.size(); i++)
		{
			glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(directionalLights[i].dir));
			offset += 16;
			glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(directionalLights[i].color));
			offset += 16;
		}
		offset += (MAX_DIRECTIONAL_LIGHTS - directionalLights.size()) * 32;

		for (unsigned int i = 0; i < otherLights.size(); i++)
		{
			glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(otherLights[i].pos));
			offset += 16;
			glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(otherLights[i].color));
			offset += 16;
			glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(otherLights[i].dir));
			offset += 16;
			glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(otherLights[i].angles));
			offset += 16;
		}

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}