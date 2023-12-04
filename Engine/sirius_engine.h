/*
	Sirius Engine
*/

#ifndef _SIRIUS_ENGINE_H_
#define _SIRIUS_ENGINE_H

#define MAX_POINT_LIGHTS 10

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "Core/camera.h"
#include "Core/object.h"
#include "Core/light.h"
#include "Core/shader.h"
#include "Assets/model.h"

// 后期特效
enum PostProcessing;

// 后期特效
enum PostProcessing;

// 引擎类
class SiriusEngine {
public:
	unsigned int framebuffer;
	Shader screenShader;
	unsigned int intermediateFBO;
	unsigned int quadVAO;
	unsigned int screenTexture;

public:
	unsigned int width, height;
	GLFWwindow* window;

	std::vector<Object*> sceneObjects;			// 现有物体表
	std::vector<PointLight*> scenePointLights;	// 现有点光源表
	Object* skybox;								// 天空盒

	Camera camera;
	DirLight dirLight;

	int currentSelectedObjectIndex;		// 当前选中物体索引
	int currentAddObjectIndex;			// 当前添加物体索引
	int currentSelectedPointLightIndex;	// 当前选中点光源索引

	bool keysPressed[1024];
	bool keysProcessed[1024];

	// 状态机
	bool isDepthTestOn;				// 深度测试
	bool isStencilTestOn;			// 模板测试
	bool isFaceCullingOn;			// 背面剔除
	bool isMouseControlOn;			// 鼠标移动
	bool isScrollControlOn;			// 滚轮操作
	bool isFreeLookingModeOn;		// 自由视角
	bool isObjectRotationModeOn;	// 镜头旋转
	bool isObjectCoordinateShown;	// 展示物体坐标
	bool isMSAAOn;					// MSAA抗锯齿
	bool isGammaOn;					// Gamma矫正


	int postProcessing;	// 后期特效

	glm::vec3 clearColor;	// 屏幕背景颜色

public:
	// 构造函数
	SiriusEngine(GLFWwindow* _window, unsigned int _width,
				 unsigned int _height);

	// 析构函数
	~SiriusEngine();

	// 初始化
	void init();

	// 处理外设输入
	void processKeyboardInput(float key);

	void processMouseInput(float xoffset, float yoffset);

	void processScrollInput(float offset);

	// 更新选中物体
	void updateSelected(float key);

	void render();

	// 更新屏幕分辨率
	void updateSceen();

private:
	// 配置渲染
	void configureRenderSetup();
};

#endif // !_SIRIUS_ENGINE_H_
