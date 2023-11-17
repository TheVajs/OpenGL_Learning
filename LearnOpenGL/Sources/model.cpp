#include "model.hpp"

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
			std::cerr << "ERROR::ASSIMP" << importer.GetErrorString() << std::endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
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
			auto diffuse = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::Diffuse);
			auto specular = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::Specular);
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
			texture.id = loadTexture(directory + '/' + relPath, true);
			texture.type = type;
			texture.path = relPath;
			textures.push_back(texture);
			texturesLoaded.push_back(texture);
		}

		return textures;
	}
}