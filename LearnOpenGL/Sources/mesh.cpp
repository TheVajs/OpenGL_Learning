#include "mesh.hpp"

#include <cstddef>

namespace Simp
{
	Mesh::Mesh(const std::vector<Vertex>& _vertices,
			const std::vector<GLuint>& _indices,
			const std::vector<Texture>& _textures)
		: vertices(_vertices), indices(_indices), textures(_textures)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv1));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	void Mesh::draw(const Shader& shader)
	{
		// TODO not the responseability of the mesh to set shader values
		// Make call to shader that initialixe textures with texture array
		// TODO make shader interface

		unsigned int diffuseNum = 0;
		unsigned int specularNum = 0;

		for (int i = 0; i < textures.size(); i++)
		{
			std::string uniform = "material.";
			if (textures[i].type == 0)
				uniform += "diffuse_texture" + std::to_string(diffuseNum++);
			else if (textures[i].type == 1)
				uniform += "specular_texture" + std::to_string(specularNum++);

			shader.setInt(uniform, i);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}