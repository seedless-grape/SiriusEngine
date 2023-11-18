#include "texture.h"

Texture::Texture() :
	width(0), height(0), internalFormat(GL_RGB), imageFormat(GL_RGB),
	wrapS(GL_REPEAT), wrapT(GL_REPEAT),
	filterMin(GL_LINEAR_MIPMAP_LINEAR), filterMax(GL_LINEAR) {
	glGenTextures(1, &this->ID);
}

void Texture::generate(unsigned int _width, unsigned int _height,
					   const unsigned char* data) {
	width = _width;
	height = _height;

	// 创建纹理
	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, width, height,
				 0, this->imageFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// 配置纹理属性
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax);

	// 解绑
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind() const {
	glBindTexture(GL_TEXTURE_2D, this->ID);
}
