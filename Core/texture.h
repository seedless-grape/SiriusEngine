/*
	����
*/

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <glad/glad.h>
#include <string>
#include <vector>

// ������
class Texture {
public:
	// ����ID
	unsigned ID;

public:
	// ���캯��
	Texture();

	// ��������
	void generate(const char* file);

	void generate(const std::vector<std::string>& faces);

	// ������
	void bind(GLenum typeTexture = GL_TEXTURE_2D) const;
};

#endif // !TEXTURE_H_
