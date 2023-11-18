/*
	纹理
*/

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <glad/glad.h>

// 纹理类
class Texture {
public:
	// 纹理ID
	unsigned ID;

	// 纹理尺寸
	unsigned int width;
	unsigned int height;

	// 纹理格式
	unsigned int internalFormat;
	unsigned int imageFormat;

	// 纹理配置
	unsigned int wrapS;
	unsigned int wrapT;
	unsigned int filterMin;
	unsigned int filterMax;

public:
	// 构造函数
	Texture();

	// 生成纹理
	void generate(unsigned int _width, unsigned int _height,
				  const unsigned char* data);

	// 绑定纹理
	void bind() const;
};

#endif // !TEXTURE_H_
