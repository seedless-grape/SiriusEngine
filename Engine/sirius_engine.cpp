#include "sirius_engine.h"

#include "Gui/gui.h"
#include "Core/object.h"
#include "Core/renderer.h"
#include "Core/light.h"
#include "Assets/resource_manager.h"
#include "Assets/loadPresets.h"
#include "Assets/modelRenderer.h"


#include <iostream>
#include <vector>

GUI* gui;
CubeRenderer* cubeRenderer;
ModelRenderer* modelRenderer;

SiriusEngine::SiriusEngine(GLFWwindow* _window, unsigned int _width,
                           unsigned int _height) :
    window(_window), width(_width), height(_height),
    camera(), dirLight(), keysPressed(), keysProcessed(),
    // 状态机
    isDepthTestOn(true), isStencilTestOn(false), isFaceCullingOn(false),
    isMouseControlOn(true), isScrollControlOn(true),
    isFreeLookingModeOn(false), isObjectRotationModeOn(false),
    isObjectCoordinateShown(true),
    postProcessing(original),
    currentSelectedObjectIndex(-1),
    currentAddObjectIndex(0),
    currentSelectedPointLightIndex(-1),
    clearColor(0.1f, 0.1f, 0.1f) { }

SiriusEngine::~SiriusEngine() {
    delete gui;
    delete cubeRenderer;
    delete modelRenderer;

    // 清空屏幕内物体
    for (unsigned int i = 0; i < sceneObjects.size(); i++)
        delete sceneObjects[i];
    sceneObjects.clear();

    // 清空屏幕内点光源
    for (unsigned int i = 0; i < scenePointLights.size(); i++)
        delete scenePointLights[i];
    scenePointLights.clear();
}

void SiriusEngine::init() {
    // 加载着色器
    ResourceManager::loadShader("Shader/light_cube.vert",
                                "Shader/light_cube.frag",
                                nullptr, "light_cube");
    ResourceManager::loadShader("Shader/object.vert",
                                "Shader/object.frag",
                                nullptr, "object");
    ResourceManager::loadShader("Shader/model.vert",
                                "Shader/model.frag",
                                nullptr, "model");

    // 加载材质
    ResourceManager::loadTexture("Resources/textures/container2.png",
                                 true, "container_diffuse");
    ResourceManager::loadTexture("Resources/textures/container2_specular.png",
                                 true, "container_specular");
    ResourceManager::loadTexture("Resources/textures/white_tiles.jpg",
                                 false, "white_tiles");
    ResourceManager::loadTexture("Resources/textures/pure_black.jpg",
                                 false, "pure_black");
    ResourceManager::loadTexture("Resources/textures/brickwall.jpg",
                                 false, "brick_wall_diffuse");

    // 模型渲染

    Object* objectModel;
    objectModel = LoadPresets::loadModel(duck_model, u8"小黄鸭");
    modelRenderer = new ModelRenderer(ResourceManager::getShader("model"));
    sceneObjects.push_back(objectModel);

    // 渲染
    cubeRenderer = new CubeRenderer(ResourceManager::getShader("object"));

    // 点光源
    PointLight* pointLight;
    pointLight = LoadPresets::loadPointLight();
    scenePointLights.push_back(pointLight);


    // 选中物体
    currentSelectedObjectIndex = sceneObjects.size() ? 0 : -1;

    // 选中点光源
    currentSelectedPointLightIndex = scenePointLights.size() ? 0 : -1;

    // gui
    gui = new GUI(*this);
    gui->initStyle();
}

void SiriusEngine::render() {
    this->configureRenderSetup();

    // 投影变换矩阵
    glm::mat4 spaceMatrix =
        camera.getProjectionMatrix(this->width, this->height) * camera.getViewMatrix();

    // 更新渲染管线
    cubeRenderer->updateRenderer(spaceMatrix, camera.position,
                                 dirLight, scenePointLights);

    modelRenderer->updateRenderer(spaceMatrix, camera.position,
                                  dirLight, scenePointLights);

    cubeRenderer->postProcessing = this->postProcessing;

    modelRenderer->postProcessing = this->postProcessing;

    // 物体绘制
    for (unsigned int i = 0; i < sceneObjects.size(); i++) {
        if (sceneObjects[i]->enabled) {
            sceneObjects[i]->draw(*modelRenderer, isObjectCoordinateShown);
        }
    }

    // 光源方块绘制
    for (unsigned int i = 0; i < scenePointLights.size(); i++) {
        if (scenePointLights[i]->enabled) {
            scenePointLights[i]->draw(*cubeRenderer);
        }
    }

    // gui
    gui->render();
}

void SiriusEngine::processKeyboardInput(float key) {
    // 窗口关闭
    if (keysPressed[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);

    // WASD移动
    if (keysPressed[GLFW_KEY_W])
        camera.processKeyboard(FORWARD, key);
    if (keysPressed[GLFW_KEY_S])
        camera.processKeyboard(BACKWARD, key);
    if (keysPressed[GLFW_KEY_A])
        camera.processKeyboard(LEFT, key);
    if (keysPressed[GLFW_KEY_D])
        camera.processKeyboard(RIGHT, key);

    // Space & Ctrl移动
    if (keysPressed[GLFW_KEY_SPACE])
        camera.processKeyboard(UP, key);
    if (keysPressed[GLFW_KEY_LEFT_CONTROL])
        camera.processKeyboard(DOWN, key);

    // 自由视角
    this->isFreeLookingModeOn =
        keysPressed[GLFW_MOUSE_BUTTON_RIGHT];

    // 镜头旋转
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
    if (isFaceCullingOn) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    } else
        glDisable(GL_CULL_FACE);

    if (isDepthTestOn)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    if (isStencilTestOn)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);
}