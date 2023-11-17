#include "shader.hpp"

namespace Simp
{
	Shader& Shader::use()
	{
		glUseProgram(id);
		return *this;
	}

	Shader& Shader::attach(const std::string& fileName)
	{
		const std::string path = PROJECT_SOURCE_DIR "/LearnOpenGL/Shaders/";

		std::string code;
		std::ifstream vShaderFile;
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			vShaderFile.open(path + fileName);
			std::stringstream vShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			code = vShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: "
				<< path + fileName << std::endl;
		}
		vShaderFile.close();

		GLuint shader = create(fileName);

		const char* codePointer = code.c_str();
		glShaderSource(shader, 1, &codePointer, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE)
		{
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
			std::unique_ptr<char[]> infoLog(new char[length]);
			glGetShaderInfoLog(shader, length, NULL, infoLog.get());
			std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED " 
				<< fileName << "\n" << infoLog.get() << std::endl;
		}

		glAttachShader(id, shader);
		glDeleteShader(shader);
		return *this;
	}

	GLuint Shader::create(const std::string& fileName)
	{
		auto index = fileName.rfind(".");
		auto ext = fileName.substr(index + 1);
		if (ext == "comp") return glCreateShader(GL_COMPUTE_SHADER);
		else if (ext == "frag") return glCreateShader(GL_FRAGMENT_SHADER);
		else if (ext == "geom") return glCreateShader(GL_GEOMETRY_SHADER);
		else if (ext == "vert") return glCreateShader(GL_VERTEX_SHADER);
		else                    return false;
	}

	Shader& Shader::link()
	{
		glLinkProgram(id);
		glGetProgramiv(id, GL_LINK_STATUS, &status);
		if (status != GL_TRUE)
		{
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
			std::unique_ptr<char[]> infoLog(new char[length]);
			glGetProgramInfoLog(id, length, NULL, infoLog.get());
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				<< infoLog.get() << std::endl;
		}
		assert(status);
		return *this;
	}

	void Shader::bind(GLuint location, GLuint value)
	{
		glUniform1ui(location, value);
	}

	void Shader::bind(GLuint location, int value)
	{
		glUniform1i(location, value);
	}

	void Shader::bind(GLuint location, bool value)
	{
		bind(location, (int)value);
	}

	void Shader::bind(GLuint location, float value)
	{
		glUniform1f(location, value);
	}

	void Shader::bind(GLuint location, const glm::vec3& value)
	{
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::bind(GLuint location, const glm::mat3& value)
	{
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::bind(GLuint location, const glm::mat4& value)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}