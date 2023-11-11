#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#if DEBUG_ASSIMP
#include <assimp/DefaultLogger.hpp>
#endif

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

		~Model()
		{
#if DEBUG_ASSIMP
			Assimp::DefaultLogger::kill();
#endif
		}

		void draw(const Shader& shader);
	private:
		std::vector<std::unique_ptr<Mesh>> meshes;
		std::vector<Texture> texturesLoaded;
		std::string directory;

		void processNode(const aiNode* node, const aiScene* scene);
		void processMesh(const aiMesh* mesh, const aiScene* scene);

		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureType type);

		Model(Model const&) = delete;
		Model& operator=(Model const&) = delete;
	};
}