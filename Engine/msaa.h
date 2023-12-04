/*
msaa：用于实现MSAA抗锯齿功能
	创建一个framebuffer，图像在渲染出来后，进入屏幕前先进入帧缓冲区
	该缓冲区为4倍帧采样，经过超采样处理后输送到屏幕空间
*/

#ifndef MSAA_H_
#define MSAA_H_

#include "Core/shader.h"

class MSAA {
public:
	unsigned int MSAAwidth;
	unsigned int MSAAheight;
	bool enable;	// 是否处于启用状态

public:
	// 构造/析构函数
	MSAA();

	~MSAA() = default;

	// MSAA开启/关闭
	void turnON(unsigned int width, unsigned int height);

	void turnOFF();

	// 屏幕分辨率更新
	void updateSceen(unsigned int width, unsigned int height);

	// 配置MSAA绘制场景
	void configureMSAASceenSetup();

	// 渲染MSAA帧空间到屏幕空间
	void render();

private:
	unsigned int VAO;	// MSAA缓冲帧空间属性VAO
	unsigned int MSAAFBO;	// MSAA帧缓冲区
	unsigned int intermediateFBO;	// 中间帧缓冲区
	unsigned int screenTexture;	// 屏幕纹理
	Shader shader;

private:
	// 创建MSAA空间VAO
	unsigned int setupVAO();

	// 配置MSAA帧缓冲区
	unsigned int configureMSAAFBO();

	// 配置中间帧缓冲区
	unsigned int configureIntermediateFBO();

	// 配置屏幕纹理
	unsigned int configureSceenTexture();

	// 配置Shader
	void configureShader();
};

#endif // !MSAA_H_

