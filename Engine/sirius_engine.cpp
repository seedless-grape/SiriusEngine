#include "sirius_engine.h"

#include "Gui/gui.h"
#include "Core/object.h"
#include "Core/renderer.h"
#include "Core/light.h"
#include "Assets/resource_manager.h"
#include "Assets/loadPresets.h"
#include "Assets/modelRenderer.h"
#include "Assets/skybox.h"
#include "msaa.h" 

#include <iostream>
#include <vector>

GUI* gui;
CubeRenderer* cubeRenderer;
ModelRenderer* modelRenderer;
SkyboxRenderer* skyboxRenderer;
MSAA* msaa;

SiriusEngine::SiriusEngine(GLFWwindow* _window, unsigned int _width,
                           unsigned int _height) :
    window(_window), width(_width), height(_height),
    camera(), dirLight(), keysPressed(), keysProcessed(),
    // ״̬��
    isDepthTestOn(true), isStencilTestOn(false), isFaceCullingOn(false),
    isMouseControlOn(true), isScrollControlOn(true),
    isFreeLookingModeOn(false), isObjectRotationModeOn(false),
    isObjectCoordinateShown(true), isMSAAOn(false), isGammaOn(true),
    postProcessing(original),
    currentSelectedObjectIndex(-1),
    currentAddObjectIndex(0),
    currentSelectedPointLightIndex(-1),
    clearColor(0.1f, 0.1f, 0.1f) { }

SiriusEngine::~SiriusEngine() {
    delete gui;
    delete cubeRenderer;
    delete modelRenderer;
    delete skyboxRenderer;
    delete msaa;

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
    // Ԥ����(ģ�͡����ʡ���ɫ��)
    LoadPresets::preLoad();

    // ģ����Ⱦ
    Object* objectModel;
    objectModel = LoadPresets::loadModel(duck_model, u8"С��Ѽ");
    modelRenderer = new ModelRenderer(ResourceManager::getShader("model"));
    sceneObjects.push_back(objectModel);

    // ��պ�����Դ������Ⱦ
    skybox = LoadPresets::loadSkybox();
    skyboxRenderer = new SkyboxRenderer(ResourceManager::getShader("skybox"));

    cubeRenderer = new CubeRenderer(ResourceManager::getShader("light_cube"));

    // ���Դ
    PointLight* pointLight;
    pointLight = LoadPresets::loadPointLight();
    scenePointLights.push_back(pointLight);

    // ѡ������
    currentSelectedObjectIndex = sceneObjects.size() ? 0 : -1;

    // ѡ�е��Դ
    currentSelectedPointLightIndex = scenePointLights.size() ? 0 : -1;

    // �����
    msaa = new MSAA();
    msaa->turnON(this->width, this->height);

    // gui
    gui = new GUI(*this);
    gui->initStyle();
}

void SiriusEngine::render() {
    this->configureRenderSetup();

    // MSAA֡��Ļ����ռ�����
    if (isMSAAOn)
        msaa->configureMSAASceenSetup();

    // ͶӰ�任����
    glm::mat4 projectionMatrix = camera.getProjectionMatrix(this->width, this->height);
    glm::mat4 spaceMatrix = projectionMatrix * camera.getViewMatrix();
    glm::mat4 skyboxSpaceMatrix = projectionMatrix * glm::mat4(glm::mat3(camera.getViewMatrix()));

    // ������Ⱦ����
    cubeRenderer->updateRenderer(spaceMatrix, camera.position,
                                 dirLight, scenePointLights);

    modelRenderer->updateRenderer(spaceMatrix, camera.position,
                                  dirLight, scenePointLights);

    skyboxRenderer->updateRenderer(skyboxSpaceMatrix);

    cubeRenderer->postProcessing = this->postProcessing;

    modelRenderer->postProcessing = this->postProcessing;

    skyboxRenderer->postProcessing = this->postProcessing;

    // ��պл���
    skybox->draw(*skyboxRenderer, false, this->isGammaOn);

    // �������
    for (unsigned int i = 0; i < sceneObjects.size(); i++) {
        if (sceneObjects[i]->enabled) {
            sceneObjects[i]->draw(*modelRenderer, isObjectCoordinateShown, this->isGammaOn);
        }
    }

    // ��Դ�������
    for (unsigned int i = 0; i < scenePointLights.size(); i++) {
        if (scenePointLights[i]->enabled) {
            scenePointLights[i]->draw(*cubeRenderer, this->isGammaOn);
        }
    }

    // MSAA֡����ռ�д����Ļ�ռ�
    if (isMSAAOn)
        msaa->render();

    // gui
    gui->render();
}

void SiriusEngine::updateSceen() {
    if (isMSAAOn) {
        msaa->updateSceen(width, height);
    }
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

void SiriusEngine::configureRenderSetup() {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // ���ڴ���
    if (isFaceCullingOn) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    } else
        glDisable(GL_CULL_FACE);
    
    // ���/ģ�����
    if (isDepthTestOn)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    if (isStencilTestOn)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);

    // �����
    if (isMSAAOn && !msaa->enable)
        msaa->turnON(width, height);
    else if (!isMSAAOn && msaa->enable)
        msaa->turnOFF();
}