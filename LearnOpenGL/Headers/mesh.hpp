#pragma once

#include <glm/glm.hpp>

#include <vector>
#include "shader.hpp"

namespace Simp
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv1;
	};

	enum TextureType
	{
		Diffuse = 0,
		Specular = 1,
	};

	struct Texture
	{
		GLuint id;
		TextureType type;
		std::string path;
	};

	class Mesh
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		Mesh(const std::vector<Vertex>& _vertices,
			 const std::vector<GLuint>& _indices,
			 const std::vector<Texture>& _textures);

		~Mesh() { glDeleteVertexArrays(1, &vao); }

		void draw(const Shader& shader);

	private:

		// Disable Copying and Assignment
		// Mesh(Mesh const&) = delete;
		// Mesh& operator=(Mesh const&) = delete;

		GLuint vao;
		GLuint vbo;
		GLuint ebo;
	};
}