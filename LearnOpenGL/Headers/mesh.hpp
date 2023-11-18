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
		Normal = 2
	};
	
	const unsigned int DIFFUSE = 0x00000001u;
	const unsigned int SPECULAR = 0x00000002u;
	const unsigned int NORMAL = 0x00000004u;

#pragma pack(push, 1)
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec3 tangent;
		glm::vec3 bitangent;
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