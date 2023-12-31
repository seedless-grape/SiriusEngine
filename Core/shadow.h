/*
shadow：用于实现物体阴影映射
	为每个光源生成一张深度贴图，在摄像机空间中渲染该贴图
	比较深度大于深度贴图的像素位置为阴影位置
*/

#ifndef SHADOW_H_
#define SHADOW_H_

#include "Core/shader.h"
#include "Core/renderer.h"

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

// 阴影类
class Shadow {
public:
	bool isShadowOn;	// 是否开启阴影
	bool isBias;		// 是否开启偏移优化
	bool isCull;		// 是否开启正面剔除
	bool isSoft;		// 是否开启PCF软阴影

public:
	// 构造/析构函数
	Shadow(unsigned int _width = SHADOW_WIDTH,
		   unsigned int _height = SHADOW_HEIGHT);

	~Shadow() = default;

	// 获取阴影贴图ID
	unsigned int getDepthMap();

	// 设置光源空间位置
	void setLightSpaceMatrix(glm::mat4 lightSpaceMatrix);

	// 绑定阴影帧缓冲区
	void bindShadowFBO();

	// 恢复默认帧缓冲区
	void unbindShadowFBO(unsigned int FBO, unsigned int width, unsigned int height);

private:
	unsigned int width;
	unsigned int height;

	Shader shader;				// 深度信息着色器
	unsigned int depthMapFBO;	// 阴影贴图帧缓冲区
	unsigned int depthMap;		// 阴影贴图

private:
	// 配置深度贴图帧缓冲区
	void configureDepthFBO();

};

class ShadowRenderer : public Renderer {
public:
	// 构造/析构函数
	ShadowRenderer(const Shader& shader);

	~ShadowRenderer() override = default;

	// 渲染
	void render(const Object& object, bool drawCoordinate = false, bool gamma = false) override;
};


#endif // !SHADOW_H_

