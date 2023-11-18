#include "sirius_engine.h"

#include "Gui/gui.h"
#include "Core/object.h"
#include "Core/renderer.h"
#include "Core/light.h"
#include "Assets/resource_manager.h"
#include "Assets/loadPresets.h"

#include <iostream>
#include <vector>

GUI* gui;
CubeRenderer* cubeRenderer;

SiriusEngine::SiriusEngine(GLFWwindow* _window, unsigned int _width,
                           unsigned int _height) :
    window(_window), width(_width), height(_height),
    camera(), dirLight(), keysPressed(), keysProcessed(),
    // ״̬��
    isDepthTestOn(true), isStencilTestOn(false),
    isMouseControlOn(true), isScrollControlOn(true),
    isFreeLookingModeOn(false), isObjectRotationModeOn(false),
    isObjectCoordinateShown(true),
    currentSelectedObjectIndex(-1),
    currentAddObjectIndex(0),
    currentSelectedPointLightIndex(-1),
    clearColor(0.1f, 0.8f, 0.1f) { }

SiriusEngine::~SiriusEngine() {
    delete gui;
    delete cubeRenderer;

    // �����Ļ������
    for (unsigned int i = 0; i < sceneObjects.size(); i++)
        delete sceneObjects[i];
    sceneObjects.clear();

    // �����Ļ�ڵ��Դ
    for (unsigned int i = 0; i < scenePointLights.size(); i++)
        delete scenePointLights[i];
    scenePointLights.clear();
}

void SiriusEngine::init() {
    // ������ɫ��
    ResourceManager::loadShader("Shader/light_cube.vert",
                                "Shader/light_cube.frag",
                                nullptr, "light_cube");
    ResourceManager::loadShader("Shader/object.vert",
                                "Shader/object.frag",
                                nullptr, "object");

    // ���ز���
    ResourceManager::loadTexture("Resources/textures/container2.png",
                                 true, "container_diffuse");
    ResourceManager::loadTexture("Resources/textures/container2_specular.png",
                                 true, "container_specular");
    ResourceManager::loadTexture("Resources/textures/brickwall.jpg",
                                 false, "brick_wall_diffuse");

    // ��Ⱦ
    cubeRenderer = new CubeRenderer(ResourceManager::getShader("object"));

    // ���Դ
    PointLight* pointLight;
    pointLight = LoadPresets::loadPointLight();
    scenePointLights.push_back(pointLight);

    // ����
    Object* cube;
    cube = LoadPresets::loadCube(Wooden_Box, u8"ľ��");
    sceneObjects.push_back(cube);

    cube = new Cube("Brick Cube");
    cube->diffuseTexture = ResourceManager::getTexture("brick_wall_diffuse");
    cube->position = glm::vec3(1.0f, 1.0f, 1.0f);
    cube->enabled = false;
    sceneObjects.push_back(cube);

    // ѡ������
    currentSelectedObjectIndex = sceneObjects.size() ? 0 : -1;

    // ѡ�е��Դ
    currentSelectedPointLightIndex = scenePointLights.size() ? 0 : -1;

    // gui
    gui = new GUI(*this);
    gui->initStyle();
}

void SiriusEngine::processKeyboardInput(float key) {
    // ���ڹر�
    if (keysPressed[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);

    // WASD�ƶ�
    if (keysPressed[GLFW_KEY_W])
        camera.processKeyboard(FORWARD, key);
    if (keysPressed[GLFW_KEY_S])
        camera.processKeyboard(BACKWARD, key);
    if (keysPressed[GLFW_KEY_A])
        camera.processKeyboard(LEFT, key);
    if (keysPressed[GLFW_KEY_D])
        camera.processKeyboard(RIGHT, key);

    // Space & Ctrl�ƶ�
    if (keysPressed[GLFW_KEY_SPACE])
        camera.processKeyboard(UP, key);
    if (keysPressed[GLFW_KEY_LEFT_CONTROL])
        camera.processKeyboard(DOWN, key);

    // �����ӽ�
    this->isFreeLookingModeOn =
        keysPressed[GLFW_MOUSE_BUTTON_RIGHT];

    // ��ͷ��ת
    this->isObjectRotationModeOn =
        keysPressed[GLFW_MOUSE_BUTTON_LEFT] &&
        keysPressed[GLFW_KEY_LEFT_SHIFT];

    camera.processRotationUpdate();
}

void SiriusEngine::processMouseInput(float xoffset, float yoffset) {
    if (this->isMouseControlOn && this->isFreeLookingModeOn)
        camera.processMouseMovement(xoffset, yoffset);
    if (this->isMouseControlOn && this->isObjectRotationModeOn) {
        sceneObjects[this->currentSelectedObjectIndex]->rotation.y += xoffset * 0.1f;
        sceneObjects[this->currentSelectedObjectIndex]->rotation.x -= yoffset * 0.1f;
    }
}

void SiriusEngine::processScrollInput(float offset) {
    if (this->isScrollControlOn && this->isFreeLookingModeOn)
        camera.processMouseScroll(offset);
}

void SiriusEngine::updateSelected(float key) {
    for (unsigned int i = 0; i < sceneObjects.size(); ++i)
        sceneObjects[i]->selected = (this->currentSelectedObjectIndex == i);
}

void SiriusEngine::render() {
    this->configureRenderSetup();

    // ͶӰ�任����
    glm::mat4 spaceMatrix =
        camera.getProjectionMatrix(this->width, this->height) * camera.getViewMatrix();

    // ������Ⱦ����
    cubeRenderer->updateRenderer(spaceMatrix, camera.position,
                                 dirLight, scenePointLights);

    // �������
    for (unsigned int i = 0; i < sceneObjects.size(); i++) {
        if (sceneObjects[i]->enabled) {
            sceneObjects[i]->draw(*cubeRenderer, isObjectCoordinateShown);
        }
    }

    // ��Դ�������
    for (unsigned int i = 0; i < scenePointLights.size(); i++) {
        if (scenePointLights[i]->enabled) {
            scenePointLights[i]->draw(*cubeRenderer);
        }
    }

    // gui
    gui->render();
}

void SiriusEngine::configureRenderSetup() {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (isDepthTestOn)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    if (isStencilTestOn)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);
}