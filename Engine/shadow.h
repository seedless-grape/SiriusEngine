/*
shadow：用于实现物体阴影映射
	为每个光源生成一张深度贴图，在摄像机空间中渲染该贴图
	比较深度大于深度贴图的像素位置为阴影位置
*/


#ifndef SHADOW_H_
#define SHADOW_H_

#include "Core/shader.h"

// 阴影类
class Shadow {
public:
	Shader shader;				// 深度信息着色器
	unsigned int depthMapFBO;	// 阴影贴图帧缓冲区
	unsigned int depthMap;		// 阴影贴图

	unsigned int width;
	unsigned int height;

public:
	// 构造/析构函数
	Shadow();

	~Shadow() = default;

	// 设置光源空间位置
	void setLightSpaceMatrix(glm::mat4 lightSpaceMatrix);


private:


private:
	// 配置深度贴图帧缓冲区
	void configureDepthFBO();

};


#endif // !SHADOW_H_

