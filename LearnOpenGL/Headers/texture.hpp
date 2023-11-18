#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
// To use stb_image, add this in *one* C++ source file.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Simp
{
	GLuint getFormat(int channelNum)
	{
		GLuint format{ GL_RED };
		switch (channelNum)
		{
		case 1: format = GL_ALPHA;     break;
		case 2: format = GL_LUMINANCE; break;
		case 3: format = GL_RGB;       break;
		case 4: format = GL_RGBA;      break;
		}
		return 	format;
	}

	GLuint loadTexture(const std::string& path, bool flip = true)
	{
		GLuint texture;

		int width;
		int height;
		int channelNum;

		stbi_set_flip_vertically_on_load(flip);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channelNum, 0);
		if (data == nullptr)
		{
			std::cerr << "WARNING::Failed to load image! " << path << std::endl;
			stbi_image_free(data);
			return 0;
		}

		GLuint format = getFormat(channelNum);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		GLint wrap = GL_REPEAT;
		if (format == GL_RGBA || format == GL_ALPHA)
		{
			wrap = GL_CLAMP_TO_EDGE;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);

		return texture;
	}

	GLuint loadCubemap(const std::vector<std::string> images, bool flip = true)
	{
		GLuint handle;
		GLuint format;

		glGenTextures(0, &handle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, handle);

		int width;
		int height;
		int channelNum;

		stbi_set_flip_vertically_on_load(flip);

		for (int i = 0; i < images.size(); i++)
		{
			unsigned char* data = stbi_load(images[i].c_str(), &width, &height, &channelNum, 0);
			format = getFormat(channelNum);

			if (data == nullptr)
			{
				std::cerr << "WARNING::Failed to load cube map image!" << std::endl;
			}
			else
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			}

			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		return handle;
	}
}