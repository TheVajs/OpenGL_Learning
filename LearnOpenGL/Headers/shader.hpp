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
		Shader(const std::string& vertex, const std::string& fragment)
		{
			std::string vertexCode, fragmentCode;

			std::ifstream vShaderFile;
			std::ifstream fShaderFile;
			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			std::string path = PROJECT_SOURCE_DIR "/LearnOpenGL/Shaders/";
			try
			{
				vShaderFile.open(path + vertex);
				fShaderFile.open(path + fragment);
				std::stringstream vShaderStream, fShaderStream;
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();

				vShaderFile.close();
				fShaderFile.close();

				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
			}
			catch (std::ifstream::failure e)
			{
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ " <<
					path + vertex << "\n" << path + fragment << std::endl;
			}

			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();

			unsigned int vertexShader, fragmentShader;

			vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &vShaderCode, NULL);
			glCompileShader(vertexShader);

			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
			if (!status)
			{
				glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
				std::unique_ptr<char[]> infoLog(new char[length]);
				glGetShaderInfoLog(vertexShader, length, NULL, infoLog.get());
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED " <<
					vertex << "\n" << infoLog.get() << std::endl;
			}

			fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
			glCompileShader(fragmentShader);

			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
			if (!status)
			{
				glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
				std::unique_ptr<char[]> infoLog(new char[length]);
				glGetShaderInfoLog(fragmentShader, length, NULL, infoLog.get());
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED " <<
					fragment << "\n" << infoLog.get() << std::endl;
			}

			id = glCreateProgram();
			glAttachShader(id, vertexShader);
			glAttachShader(id, fragmentShader);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
		}

		Shader& link()
		{
			glLinkProgram(id);
			glGetProgramiv(id, GL_LINK_STATUS, &status);
			if (!status)
			{
				glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
				std::unique_ptr<char[]> infoLog(new char[length]);
				glGetProgramInfoLog(id, length, NULL, infoLog.get());
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
					<< infoLog.get() << std::endl;
			}
			assert(status);
			return *this;
		}

		void use()
		{
			glUseProgram(id);
		}

		void setBool(const std::string& name, bool value) const
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
		}

		void setInt(const std::string& name, int value) const
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), value);
		}

		void setFloat(const std::string& name, float value) const
		{
			glUniform1f(glGetUniformLocation(id, name.c_str()), value);
		}

		void setVec3(const std::string& name, glm::vec3 value) const
		{
			glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
		}

		void setMat4(const std::string& name, glm::mat4& rts)
		{
			glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(rts));
			// 1 matrix to send, GL_FALSE don't transpose matrxi, collumn major order in GLM
		}

		void setMat3(const std::string& name, glm::mat3& rts)
		{
			glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(rts));
		}

	private:
		// Disable Copying and Assignment
		Shader(Shader const&) = delete;
		Shader& operator=(Shader const&) = delete;

		GLuint id;
		GLint status;
		GLint length;
	};
}