#include "world.hpp"

#include <iostream>

namespace Simp
{
	OtherLight::OtherLight(glm::vec4 _pos, glm::vec3 _color) :
		pos(_pos), color(_color), dir(glm::vec3(1.0f, 0.0f, 0.0f)), angles(calculateSpotAngle(360.0f, 360.0f))
	{
	}

	OtherLight::OtherLight(glm::vec4 _pos, glm::vec3 _color, glm::vec3 _dir, float outter, float inner)
		: pos(_pos), color(_color), dir(_dir), angles(calculateSpotAngle(outter, inner))
	{
	}

	const std::vector<std::unique_ptr<DirectionalLight>>& World::getDirectionalLights() const
	{
		return directionalLights;
	}

	const std::vector<std::unique_ptr<OtherLight>>& World::getOtherLights() const
	{
		return otherLights;
	}

	glm::vec2 OtherLight::calculateSpotAngle(float outter, float inner) const
	{
		float outCos = glm::cos(glm::radians(outter) * 0.5f);
		float inCos = glm::cos(glm::radians(inner) * 0.5f);
		float invRange = 1.0f / fmax(inCos - outCos, 1e-5f);
		return glm::vec2(invRange, -outCos * invRange);
	}

	World::World()
	{
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, uboSize, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	World& World::attachLight(std::unique_ptr<DirectionalLight>& light)
	{
		SIMP_ASSERT(directionalLights.size() < MAX_DIRECTIONAL_LIGHTS);
		directionalLights.push_back(std::move(light));
		return *this;
	}

	World& World::attachLight(std::unique_ptr<OtherLight>& light)
	{
		SIMP_ASSERT(otherLights.size() < MAX_OTHER_LIGHTS);
		otherLights.push_back(std::move(light));
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

	void World::bindLights()
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
			glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(directionalLights[i].get()->dir));
			offset += 16;
			glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(directionalLights[i].get()->color));
			offset += 16;
		}
		offset += (MAX_DIRECTIONAL_LIGHTS - static_cast<int>(directionalLights.size())) * 32;

		for (unsigned int i = 0; i < otherLights.size(); i++)
		{
			glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(otherLights[i].get()->pos));
			offset += 16;
			glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(otherLights[i].get()->color));
			offset += 16;
			glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(otherLights[i].get()->dir));
			offset += 16;
			glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(otherLights[i].get()->angles));
			offset += 16;
		}

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void World::drawPointLights(const Camera& camera, Shader& shader, GLuint vao, GLuint size) const
	{
		shader.use();
		glBindVertexArray(vao);
		shader.bind("view", camera.getViewMatrix());
		shader.bind("projection", camera.getProjectionMatrix());

		for (unsigned int i = 0; i < otherLights.size(); i++)
		{
			glm::mat4 model(1.0f);
			glm::vec3 position(otherLights[i].get()->pos);
			model = glm::translate(model, position);
			model = glm::scale(model, glm::vec3(0.2f));
			shader.bind("model", model);
			glDrawArrays(GL_TRIANGLES, 0, size);
		}

		glBindVertexArray(0);
	}
}