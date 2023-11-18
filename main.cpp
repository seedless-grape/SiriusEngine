/*
	Created by grape on 2023/11/11
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Engine/sirius_engine.h"

// ����ǰ������
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseButtonCallback(GLFWwindow* window, int key, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
bool checkIfRunInRetina(GLFWwindow* window, float* dpi = nullptr);

// ��Ļ����
const unsigned int SCR_WIDTH = 1920; // ��Ļ��
const unsigned int SCR_HEIGHT = 1080; // ��Ļ��

// ����
SiriusEngine* siriusEngine;

int main() {
	/* GLFW�ĳ�ʼ�������� */
	/* -------------------------------------------------------- */
	// test

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* GLFW���ڴ��� */
	/* -------------------------------------------------------- */

	// ����һ�����ڶ���
	GLFWwindow* window = glfwCreateWindow(1080, 920, "OpenGLTest", NULL, NULL);
	if (window == NULL) { // ����ʧ������ֹGLFW
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// ֪ͨGLFW�������Ĵ���window������������Ϊ��ǰ�̵߳���������
	glfwMakeContextCurrent(window);

	// ��ʼ��GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ��GLFW��ע���ӿڵ���������ÿ��ϣ���������ڴ�Сʱ�����������
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// ע��ص�����
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// �������
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/* ���� */
	/* -------------------------------------------------------- */

	siriusEngine = new SiriusEngine(window, SCR_WIDTH, SCR_HEIGHT);
	siriusEngine->init();

	/* ��Ⱦѭ�� */
	/* -------------------------------------------------------- */

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		// ��������
		glfwSetWindowSize(window, siriusEngine->width, siriusEngine->height);

		// ֡ͬ���߼�
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ������޴����¼�,���´���״̬�����ö�Ӧ�Ļص�����
		glfwPollEvents();

		// ���봦��
		siriusEngine->processKeyboardInput(deltaTime);

		// ���ݸ���
		siriusEngine->updateSelected(deltaTime);

		// ��Ⱦ
		siriusEngine->render();

		// ������ɫ����
		glfwSwapBuffers(window);
	}

	delete siriusEngine;

	return 0;
}

// �޸Ľ���ߴ�ʱ�ص�
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	// ��������ߴ�
	siriusEngine->width = checkIfRunInRetina(window) ? width / 2 : width;
	siriusEngine->height = checkIfRunInRetina(window) ? height / 2 : height;

	std::cout << "SIZE UPDATE: "
		<< siriusEngine->width << " x " << siriusEngine->height << " (window) |"
		<< width << " x " << height << " (display) " << std::endl;

	// �����ӿڳߴ�
	glViewport(0, 0, width, height);
}

// ���̼���ص�
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

// ������ص�
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

// ����ƶ��ص�
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	float fxpos = static_cast<float>(xpos);
	float fypos = static_cast<float>(ypos);

	// �������ǳ�ʼλ��
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

// �����ֻص�
void scrollCallback(GLFWwindow* window, double xoffset,
					double yoffset) {
	siriusEngine->processScrollInput(yoffset);
}

// ����Ӵ�����Ⱦ����ߴ��Ƿ����
bool checkIfRunInRetina(GLFWwindow* window, float* dpi) {
	int windowWidth, windowHeight;
	int bufferWidth, bufferHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	if (dpi != nullptr)
		*dpi = (float)bufferWidth / (float)windowWidth;
	return bufferWidth != windowWidth;
}

