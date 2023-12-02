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
#include "Core/light.h"
#include "Assets/model.h"

// ������Ч
enum PostProcessing;

// ������
class SiriusEngine {
	/*-----------------*/
public:
	unsigned int planeVAO;

	glm::vec3 lightPos;

	unsigned int cubeVAO;
	unsigned int cubeVBO;

	Texture woodTexture;

	void renderScene(Shader& shader);
	void renderCube();

	/*-----------------*/

public:
	unsigned int width, height;
	GLFWwindow* window;

	std::vector<Object*> sceneObjects;			// ���������
	std::vector<PointLight*> scenePointLights;	// ���е��Դ��
	Object* skybox;								// ��պ�

	Camera camera;
	DirLight dirLight;

	int currentSelectedObjectIndex;		// ��ǰѡ����������
	int currentAddObjectIndex;			// ��ǰ�����������
	int currentSelectedPointLightIndex;	// ��ǰѡ�е��Դ����

	bool keysPressed[1024];
	bool keysProcessed[1024];

	// ״̬��
	bool isDepthTestOn;				// ��Ȳ���
	bool isStencilTestOn;			// ģ�����
	bool isFaceCullingOn;			// �����޳�
	bool isMouseControlOn;			// ����ƶ�
	bool isScrollControlOn;			// ���ֲ���
	bool isFreeLookingModeOn;		// �����ӽ�
	bool isObjectRotationModeOn;	// ��ͷ��ת
	bool isObjectCoordinateShown;	// չʾ��������
	bool isMSAAOn;					// MSAA�����
	bool isGammaOn;					// Gamma����
	bool isShadowOn;				// ��Ӱ


	int postProcessing;	// ������Ч

	glm::vec3 clearColor;	// ��Ļ������ɫ

	Shader shader;

public:
	// ���캯��
	SiriusEngine(GLFWwindow* _window, unsigned int _width,
				 unsigned int _height);

	// ��������
	~SiriusEngine();

	// ��ʼ��
	void init();

	// ������������
	void processKeyboardInput(float key);

	void processMouseInput(float xoffset, float yoffset);

	void processScrollInput(float offset);

	// ����ѡ������
	void updateSelected(float key);

	void render();

	// ������Ļ�ֱ���
	void updateSceen();

private:
	// ������Ⱦ
	void configureRenderSetup();
};

#endif // !_SIRIUS_ENGINE_H_
