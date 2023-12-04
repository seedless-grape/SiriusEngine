/*
hdr������ʵ�ָ߶�̬��Χ����
*/

#ifndef HDR_H_
#define HDR_H_

#include "Core/shader.h"

class HDR {
public:
	unsigned int HDRwidth;
	unsigned int HDRheight;
	bool enable;

public:
	// ����/��������
	HDR();

	~HDR() = default;

	// ����/�ر�HDR
	void turnON(unsigned int width, unsigned int height);

	void turnOFF();

	// ��Ļ�ֱ��ʸ���
	void updateSceen(unsigned int width, unsigned int height);

	// ����HDR���Ƴ���
	void configureHDRSceenSetup();

	// ��ȾHDR֡�ռ䵽��Ļ�ռ�
	void render();

private:
	unsigned int HDRFBO;		// HDR֡����
	unsigned int colorBuffer;	// ��ɫ����
	unsigned int quadVAO;
	Shader shader;

private:
	// ����HDR֡������
	unsigned int configureHDRFBO();

	// ����HDR��ɫ����
	unsigned int configureHDRColorBuffer();

	// ������Ļ����
	unsigned int configureSceenTexture();

	// ����Shader
	void configureShader();

	// HDR��Ⱦ�ռ�
	void renderQuad();
};

#endif // !MSAA_H_