#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.hpp"
#include "mesh.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace Simp
{
	unsigned int loadTexture(const std::string& path);

	class Model
	{
	public:
		Model(const std::string& path);

		void draw(const Shader& shader);
	private:
		std::vector<Mesh> meshes;
		std::vector<Texture> texturesLoaded; 
		std::string directory;

		void processNode(aiNode* node, const aiScene* scene);
		Simp::Mesh processMesh(aiMesh* mesh, const aiScene* scene);

		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureType type);
	};
}