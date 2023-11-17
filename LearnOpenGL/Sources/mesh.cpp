#include "mesh.hpp"

#include <glad/glad.h>

namespace Simp
{
	Mesh::Mesh(const std::vector<Vertex>& _vertices,
			const std::vector<GLuint>& _indices,
			const std::vector<Texture>& _textures)
		: vertices(_vertices), indices(_indices), textures(_textures)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}

	void Mesh::draw(Shader& shader)
	{
		unsigned int diffuseNum = 0;
		unsigned int specularNum = 0;
		for (int i = 0; i < textures.size(); i++)
		{
			std::string uniform = "material.";
			if (textures[i].type == TextureType::Diffuse)
				uniform += "texture_diffuse" + std::to_string(diffuseNum++);
			else if (textures[i].type == TextureType::Specular)
				uniform += "texture_specular" + std::to_string(specularNum++);

			shader.bind(glGetUniformLocation(shader.getHandle(), uniform.c_str()), i);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(indices.size()), GL_UNSIGNED_INT, 0);
	}
}