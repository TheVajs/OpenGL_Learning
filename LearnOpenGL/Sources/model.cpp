#include "model.hpp"

#include <stb_image.h>

namespace Simp
{
	Model::Model(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP" << importer.GetErrorString() << std::endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void Model::draw(const Shader& shader)
	{
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i].draw(shader);
		}
	}

	void Model::processNode(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Simp::Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 temp;

			temp.x = mesh->mVertices[i].x;
			temp.y = mesh->mVertices[i].y;
			temp.z = mesh->mVertices[i].z;
			vertex.position = temp;

			if (mesh->HasNormals())
            {
				temp.x = mesh->mNormals[i].x;
				temp.y = mesh->mNormals[i].y;
				temp.z = mesh->mNormals[i].z;
				vertex.normal = temp;
			}

			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.uv1 = vec;
			}
			else
			{
				vertex.uv1 = glm::vec2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE,
				Simp::TextureType::Diffuse);
			std::vector<Texture> specularTextures = loadMaterialTextures(material, aiTextureType_SPECULAR,
				Simp::TextureType::Specular);

			textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
			textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
		}

		return Simp::Mesh(vertices, indices, textures);
	}

	std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureType type)
	{
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < mat->GetTextureCount(aiType); i++)
		{
			aiString str;
			mat->GetTexture(aiType, i, &str);
			std::string relPath = std::string(str.C_Str());

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

			if (!skip)
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