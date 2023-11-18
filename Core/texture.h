/*
	����
*/

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <glad/glad.h>

// ������
class Texture {
public:
	// ����ID
	unsigned ID;

	// ����ߴ�
	unsigned int width;
	unsigned int height;

	// �����ʽ
	unsigned int internalFormat;
	unsigned int imageFormat;

	// ��������
	unsigned int wrapS;
	unsigned int wrapT;
	unsigned int filterMin;
	unsigned int filterMax;

public:
	// ���캯��
	Texture();

	// ��������
	void generate(unsigned int _width, unsigned int _height,
				  const unsigned char* data);

	// ������
	void bind() const;
};

#endif // !TEXTURE_H_
