#pragma once

#include <glm/glm.hpp>

#include <vector>
#include "shader.hpp"

namespace Simp
{
	enum TextureType
	{
		Diffuse = 0,
		Specular = 1,
	};

#pragma pack(push, 1)
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct Texture
	{
		GLuint id;
		TextureType type;
		std::string path;
	};
#pragma pack(pop)

	class Mesh
	{
	public:
		GLuint vao;
		GLuint vbo;
		GLuint ebo;

		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		Mesh(const std::vector<Vertex>& _vertices,
			 const std::vector<GLuint>& _indices,
			 const std::vector<Texture>& _textures);

		~Mesh()
		{
			glDeleteVertexArrays(1, &vao);
		}

		void draw(Shader& shader);

	private:
		// Disable Copying and Assignment
		Mesh(Mesh const&) = delete;
		Mesh& operator=(Mesh const&) = delete;
	};
}