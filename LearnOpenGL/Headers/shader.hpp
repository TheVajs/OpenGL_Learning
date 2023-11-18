#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>

namespace Simp
{
	class Shader
	{
	public:
		Shader() { id = glCreateProgram(); }
		~Shader() { glDeleteProgram(id); }

		Shader& attach(const std::string& fileName);
		Shader& link();
		void use();

		GLuint getHandle() const { return id; }

		void bind(GLuint location, GLuint value);
		void bind(GLuint location, int value);
		void bind(GLuint location, bool value);
		void bind(GLuint location, float value);
		void bind(GLuint location, const glm::vec3& value);
		void bind(GLuint location, const glm::mat3& value);
		void bind(GLuint location, const glm::mat4& value);

		template<typename T>
		Shader& bind(const std::string& path, T&& value)
		{
			GLuint location = glGetUniformLocation(id, path.c_str());
			if (location == -1)
				std::cout << "WARNING::uniform location missing! " << path << std::endl;
			else
				bind(location, std::forward<T>(value));
			return *this;
		}

	private:
		// Disable Copying and Assignment
		Shader(Shader const&) = delete;
		Shader& operator=(Shader const&) = delete;

		GLuint create(const std::string& fileName);

		GLuint id;
		GLint status;
		GLint length;
	};
}