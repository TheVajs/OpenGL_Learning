#include "model.hpp"

#include <stb_image.h>

namespace Simp
{
	Model::Model(const std::string& path)
	{
#if DEBUG_ASSIMP
		Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
		Assimp::LogStream* stderrStream = Assimp::LogStream::createDefaultStream(aiDefaultLogStream_STDERR);
		Assimp::DefaultLogger::get()->attachStream(stderrStream, Assimp::Logger::NORMAL | Assimp::Logger::DEBUGGING | Assimp::Logger::VERBOSE);
#endif

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path,
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP" << importer.GetErrorString() << std::endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
		// const aiNode* node = scene->mRootNode->mChildren[0];
		// processMesh(scene->mMeshes[node->mMeshes[0]], scene);
	}

	void Model::draw(Shader& shader)
	{
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i].get()->draw(shader);
		}
	}

	void Model::processNode(const aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			processMesh(scene->mMeshes[node->mMeshes[i]], scene);
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	void Model::processMesh(const aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

			if (mesh->mTextureCoords[0])
			{
				vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}
			else
			{
				vertex.uv = glm::vec2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			{
				indices.push_back(mesh->mFaces[i].mIndices[j]);
			}
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			auto diffuse = loadMaterialTextures(material, aiTextureType_DIFFUSE,
				Simp::TextureType::Diffuse);
			auto specular = loadMaterialTextures(material, aiTextureType_SPECULAR,
				Simp::TextureType::Specular);

			textures.insert(textures.begin(), diffuse.begin(), diffuse.end());
			textures.insert(textures.begin(), specular.begin(), specular.end());
		}

		meshes.push_back(std::unique_ptr<Mesh>(new Mesh(vertices, indices, textures)));
	}

	std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureType type)
	{
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(aiType); i++)
		{
			aiString str;
			mat->GetTexture(aiType, i, &str);

			auto relPath = std::string(str.C_Str());
			bool skip = false;
			for (unsigned int j = 0; j < texturesLoaded.size(); j++)
			{
				if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(texturesLoaded[j]);
					skip = true;
					break;
				}
			}

			if (skip)
				return textures;

			Texture texture;
			texture.id = loadTexture(directory + '/' + relPath);
			texture.type = type;
			texture.path = relPath;
			textures.push_back(texture);
			texturesLoaded.push_back(texture);
		}

		return textures;
	}

	GLuint loadTexture(const std::string& path)
	{
		GLuint texture;
		GLenum format = GL_RED;

		int width, height;
		int channelNum;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channelNum, 0);
		if (*data == NULL)
		{
			std::cout << "WARNING::Failed to load image! " << path << std::endl;
			return 0;
		}

		switch (channelNum)
		{
		case 1: format = GL_ALPHA;     break;
		case 2: format = GL_LUMINANCE; break;
		case 3: format = GL_RGB;       break;
		case 4: format = GL_RGBA;      break;
		}

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

		return texture;
	}
}