/*
hdr：用于实现高动态范围功能
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
	// 构造/析构函数
	HDR();

	~HDR() = default;

	// 开启/关闭HDR
	void turnON(unsigned int width, unsigned int height);

	void turnOFF();

	// 屏幕分辨率更新
	void updateSceen(unsigned int width, unsigned int height);

	// 配置HDR绘制场景
	void configureHDRSceenSetup();

	// 渲染HDR帧空间到屏幕空间
	void render();

private:
	unsigned int HDRFBO;		// HDR帧缓冲
	unsigned int colorBuffer;	// 颜色缓冲
	unsigned int quadVAO;
	Shader shader;

private:
	// 配置HDR帧缓冲区
	unsigned int configureHDRFBO();

	// 配置HDR颜色缓冲
	unsigned int configureHDRColorBuffer();

	// 配置屏幕纹理
	unsigned int configureSceenTexture();

	// 配置Shader
	void configureShader();

	// HDR渲染空间
	void renderQuad();
};

#endif // !MSAA_H_