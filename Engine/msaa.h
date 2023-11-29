/*
msaa������ʵ��MSAA����ݹ���
	����һ��framebuffer��ͼ������Ⱦ�����󣬽�����Ļǰ�Ƚ���֡������
	�û�����Ϊ4��֡������������������������͵���Ļ�ռ�
*/

#ifndef MSAA_H_
#define MSAA_H_

#include "Core/shader.h"

class MSAA {
public:
	unsigned int MSAAwidth;
	unsigned int MSAAheight;
	bool enable;	// �Ƿ�������״̬

public:
	// ����/��������
	MSAA();

	~MSAA() = default;

	// MSAA����/�ر�
	void turnON(unsigned int width, unsigned int height);

	void turnOFF();

	// ��Ļ�ֱ��ʸ���
	void updateSceen(unsigned int width, unsigned int height);

	// ����MSAA���Ƴ���
	void configureMSAASceenSetup();

	// ��ȾMSAA֡�ռ䵽��Ļ�ռ�
	void render();

private:
	unsigned int VAO;	// MSAA����֡�ռ�����VAO
	unsigned int MSAAFBO;	// MSAA֡������
	unsigned int intermediateFBO;	// �м�֡������
	unsigned int screenTexture;	// ��Ļ����
	Shader shader;

private:
	// ����MSAA�ռ�VAO
	unsigned int setupVAO();

	// ����MSAA֡������
	unsigned int configureMSAAFBO();

	// �����м�֡������
	unsigned int configureIntermediateFBO();

	// ������Ļ����
	unsigned int configureSceenTexture();

	// ����Shader
	void configureShader();
};

#endif // !MSAA_H_

