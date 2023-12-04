/*
	纹理
*/

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <glad/glad.h>
#include <string>
#include <vector>

// 纹理类
class Texture {
public:
	unsigned ID;		// 纹理ID
	std::string type;	// 纹理路径
	std::string path;	// 纹理路径

public:
	// 构造函数
	Texture();

	// 生成纹理
	void generate(const char* file);

	void generate(const std::vector<std::string>& faces);

	// 绑定纹理
	void bind(GLenum typeTexture = GL_TEXTURE_2D) const;
};

#endif // !TEXTURE_H_
