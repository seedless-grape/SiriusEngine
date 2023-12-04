#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Library/stb_image.h"

#include <iostream>

Texture::Texture() {
	glGenTextures(1, &this->ID);
}

void Texture::generate(const char* file) {
	int width, height, nrComponents;
	unsigned char* data = stbi_load(file, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format, internalFormat;
		if (nrComponents == 1) {
			format = GL_RED;
			internalFormat = GL_RED;
		}
		else if (nrComponents == 3) {
			format = GL_RGB;
			internalFormat = GL_RGB16F;
		}
		else if (nrComponents == 4) {
			format = GL_RGBA;
			internalFormat = GL_RGBA16F;
		}

		// 创建纹理
		glBindTexture(GL_TEXTURE_2D, this->ID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height,
					 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// 配置纹理属性
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 解绑
		glBindTexture(GL_TEXTURE_2D, 0);
		
	} else {
		std::cout << "Texture failed to load at path: " << file << std::endl;
	}				 

	stbi_image_free(data);
}

void Texture::generate(const std::vector<std::string>& faces) {
	// 天空盒纹理特配
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->ID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data) {
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
						 width, height, 0, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		} else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Texture::bind(GLenum typeTexture) const {
	glBindTexture(typeTexture, this->ID);
}
