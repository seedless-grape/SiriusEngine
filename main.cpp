/*
	Created by grape on 2023/11/11
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Engine/sirius_engine.h"

// 函数前向声明
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseButtonCallback(GLFWwindow* window, int key, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
bool checkIfRunInRetina(GLFWwindow* window, float* dpi = nullptr);

// 屏幕设置
const unsigned int SCR_WIDTH = 1920; // 屏幕宽
const unsigned int SCR_HEIGHT = 1080; // 屏幕高

// 引擎
SiriusEngine* siriusEngine;

int main() {
	/* GLFW的初始化和配置 */
	/* -------------------------------------------------------- */
	// test

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* GLFW窗口创建 */
	/* -------------------------------------------------------- */

	// 创建一个窗口对象
	GLFWwindow* window = glfwCreateWindow(1080, 920, "OpenGLTest", NULL, NULL);
	if (window == NULL) { // 创建失败则终止GLFW
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// 通知GLFW将创建的窗口window的上下文设置为当前线程的主上下文
	glfwMakeContextCurrent(window);

	// 初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 在GLFW中注册视口调整函数，每当希望调整窗口大小时调用这个函数
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// 注册回调函数
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// 隐藏鼠标
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/* 引擎 */
	/* -------------------------------------------------------- */

	siriusEngine = new SiriusEngine(window, SCR_WIDTH, SCR_HEIGHT);
	siriusEngine->init();

	/* 渲染循环 */
	/* -------------------------------------------------------- */

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		// 窗口设置
		glfwSetWindowSize(window, siriusEngine->width, siriusEngine->height);

		// 帧同步逻辑
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 检查有无触发事件,更新窗口状态并调用对应的回调函数
		glfwPollEvents();

		// 输入处理
		siriusEngine->processKeyboardInput(deltaTime);

		// 数据更新
		siriusEngine->updateSelected(deltaTime);

		// 渲染
		siriusEngine->render();

		// 交换颜色缓冲
		glfwSwapBuffers(window);
	}

	delete siriusEngine;

	return 0;
}

// 修改界面尺寸时回调
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	// 更新引擎尺寸
	siriusEngine->width = checkIfRunInRetina(window) ? width / 2 : width;
	siriusEngine->height = checkIfRunInRetina(window) ? height / 2 : height;

	std::cout << "SIZE UPDATE: "
		<< siriusEngine->width << " x " << siriusEngine->height << " (window) |"
		<< width << " x " << height << " (display) " << std::endl;

	// 更新视口尺寸
	glViewport(0, 0, width, height);
}

// 键盘键入回调
void keyboardCallback(GLFWwindow* window, int key, int scancode,
					  int action, int mode) {
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			siriusEngine->keysPressed[key] = true;
		} else if (action == GLFW_RELEASE) {
			siriusEngine->keysPressed[key] = false;
			siriusEngine->keysProcessed[key] = false;
		}
	}
}

// 鼠标点击回调
void mouseButtonCallback(GLFWwindow* window, int key,
						 int action, int mode) {
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			siriusEngine->keysPressed[key] = true;
		} else if (action == GLFW_RELEASE) {
			siriusEngine->keysPressed[key] = false;
			siriusEngine->keysProcessed[key] = false;
		}
	}
}

// 鼠标移动回调
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	float fxpos = static_cast<float>(xpos);
	float fypos = static_cast<float>(ypos);

	// 如果鼠标是初始位置
	if (firstMouse) {
		lastX = fxpos;
		lastY = fypos;
		firstMouse = false;
	}

	float xoffset = fxpos - lastX;
	float yoffset = lastY - fypos;
	lastX = fxpos;
	lastY = fypos;

	siriusEngine->processMouseInput(xoffset, yoffset);
}

// 鼠标滚轮回调
void scrollCallback(GLFWwindow* window, double xoffset,
					double yoffset) {
	siriusEngine->processScrollInput(yoffset);
}

// 检查视窗与渲染界面尺寸是否相等
bool checkIfRunInRetina(GLFWwindow* window, float* dpi) {
	int windowWidth, windowHeight;
	int bufferWidth, bufferHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	if (dpi != nullptr)
		*dpi = (float)bufferWidth / (float)windowWidth;
	return bufferWidth != windowWidth;
}

