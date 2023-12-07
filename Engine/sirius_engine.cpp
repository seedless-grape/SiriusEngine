#include "sirius_engine.h"

#include "Gui/gui.h"
#include "Assets/resource_manager.h"
#include "Assets/loadPresets.h"
#include "Assets/skybox.h"
#include "msaa.h" 
#include "hdr.h"

#include "Library/stb_image.h"

GUI* gui;
CubeRenderer* cubeRenderer;
ModelRenderer* modelRenderer;
ModelRenderer* modelPBRRenderer;
ShadowRenderer* modelShadowRenderer;
SkyboxRenderer* skyboxRenderer;
MSAA* msaa;
HDR* hdr;

SiriusEngine::SiriusEngine(GLFWwindow* _window, unsigned int _width,
                           unsigned int _height) :
    window(_window), width(_width), height(_height),
    camera(), dirLight(), keysPressed(), keysProcessed(),
    // 状态机
    isDepthTestOn(true), isStencilTestOn(false), isFaceCullingOn(false),
    isMouseControlOn(true), isScrollControlOn(true), isHDROn(false),
    isFreeLookingModeOn(false), isObjectRotationModeOn(false), isPBROn(false),
    isObjectCoordinateShown(true), isMSAAOn(false), isGammaOn(false),
    postProcessing(original),
    currentSelectedObjectIndex(-1),
    currentAddObjectIndex(0),
    currentSelectedPointLightIndex(-1), 
    clearColor(0.1f, 0.1f, 0.1f) { 
}

SiriusEngine::~SiriusEngine() {
    delete gui;
    delete cubeRenderer;
    delete modelRenderer;
    delete modelShadowRenderer;
    delete skyboxRenderer;
    delete msaa;

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
    // Ԥ���ز��ʡ���ɫ��
    LoadPresets::preLoadShaderTexture();

    // ģ����ģ����Ӱ��Ⱦ
    LoadPresets::preLoadModel(sceneObjects);

    modelRenderer = new ModelRenderer(ResourceManager::getShader("model"));
    modelPBRRenderer = new ModelRenderer(ResourceManager::getShader("pbr"));
    modelShadowRenderer = new ShadowRenderer(ResourceManager::getShader("shadow"));

    // ��պ�����Դ������Ⱦ
    skybox = LoadPresets::loadSkybox();
    skyboxRenderer = new SkyboxRenderer(ResourceManager::getShader("skybox"));

    cubeRenderer = new CubeRenderer(ResourceManager::getShader("light_cube"));

    // 点光源
    PointLight* pointLight;
    pointLight = LoadPresets::loadPointLight();
    pointLight->position = glm::vec3(0.370f, 0.295f, -11.920f);
    scenePointLights.push_back(pointLight);

    // 定向光
    dirLight.direction = glm::vec3(-0.540f, -1.225f, -1.000f);
    dirLight.ambient = glm::vec3(0.1f);
    dirLight.diffuse = glm::vec3(0.6f);

    // 选中物体
    currentSelectedObjectIndex = sceneObjects.size() ? 0 : -1;

    // 选中点光源
    currentSelectedPointLightIndex = scenePointLights.size() ? 0 : -1;
  
    // 抗锯齿
    msaa = new MSAA();
    msaa->turnON(this->width, this->height);

    // HDR
    hdr = new HDR();
    hdr->turnON(this->width, this->height);

    // 阴影
    shadow = new Shadow();

    // gui
    gui = new GUI(*this);
    gui->initStyle();
}

void SiriusEngine::render() {
    this->configureRenderSetup();

    // 投影变换矩阵
    glm::mat4 projectionMatrix = camera.getProjectionMatrix(static_cast<float>(this->width),
                                                            static_cast<float>(this->height));
    glm::mat4 spaceMatrix = projectionMatrix * camera.getViewMatrix();
    glm::mat4 skyboxSpaceMatrix = projectionMatrix * glm::mat4(glm::mat3(camera.getViewMatrix()));

    // PBR
    ModelRenderer* currentRenderer;
    if (isPBROn)
        currentRenderer = modelPBRRenderer;
    else
        currentRenderer = modelRenderer;

    // 更新渲染管线
    cubeRenderer->updateRenderer(spaceMatrix, camera.position,
                                 dirLight, scenePointLights);

    currentRenderer->updateRenderer(spaceMatrix, camera.position,
                                  dirLight, scenePointLights);

    skyboxRenderer->updateRenderer(skyboxSpaceMatrix);

    cubeRenderer->postProcessing = this->postProcessing;

    currentRenderer->postProcessing = this->postProcessing;

    skyboxRenderer->postProcessing = this->postProcessing;

    // MSAA֡��Ļ����ռ�����
    if (isMSAAOn)
        msaa->configureMSAASceenSetup();

    // HDR֡��Ļ����ռ�����
    if (isHDROn)
        hdr->configureHDRSceenSetup();

    // ��պл���
    skybox->draw(*skyboxRenderer, nullptr, false, this->isGammaOn);

    // 光源方块绘制
    for (unsigned int i = 0; i < scenePointLights.size(); i++) {
        if (scenePointLights[i]->enabled) {
            scenePointLights[i]->draw(*cubeRenderer, this->isGammaOn);
        }
    }

    // 阴影
    if (shadow->isShadowOn) {
        currentRenderer->objectShader.setBool("shadowOn", true, true);

        // Bia
        if(shadow->isBias)
            currentRenderer->objectShader.setFloat("biasValue", 0.005f);
        else
            currentRenderer->objectShader.setFloat("biasValue", 0.000f);

        // PCF
        if (shadow->isSoft)
            currentRenderer->objectShader.setBool("softShadow", true);
        else
            currentRenderer->objectShader.setBool("softShadow", false);

        // 正面剔除
        if (shadow->isCull) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
        }

		// 定向光计算位置
		dirLight.updatePosition();

		// 光照位置空间
		glm::mat4 dirLightSpaceMatrix = dirLight.getLightSpaceMatrix();
        shadow->setLightSpaceMatrix(dirLightSpaceMatrix);
        currentRenderer->objectShader.setMat4("lightSpaceMatrix", dirLightSpaceMatrix, true);

        shadow->bindShadowFBO();

        // 渲染阴影
        for (unsigned int i = 0; i < sceneObjects.size(); i++)
            if (sceneObjects[i]->enabled)
                sceneObjects[i]->shadowDraw(*modelShadowRenderer);

        if (shadow->isCull) {
            glCullFace(GL_BACK);
            if (!isFaceCullingOn)
                glDisable(GL_CULL_FACE);
        }

        shadow->unbindShadowFBO(0, this->width, this->height);

        // 渲染物体
        for (unsigned int i = 0; i < sceneObjects.size(); i++)
            if (sceneObjects[i]->enabled)
                sceneObjects[i]->draw(*currentRenderer, shadow, isObjectCoordinateShown, isGammaOn);
    } else { // 不开启阴影，直接渲染
        currentRenderer->objectShader.setBool("shadowOn", false, true);

        // 渲染物体
        for (unsigned int i = 0; i < sceneObjects.size(); i++)
            if (sceneObjects[i]->enabled)
                sceneObjects[i]->draw(*currentRenderer, nullptr, isObjectCoordinateShown, this->isGammaOn);
    }

    // MSAA֡
    if (isMSAAOn)
        msaa->render();

    // HDR֡
    if (isHDROn) {
        hdr->render();
    }

    // gui
    gui->render();
}

void SiriusEngine::updateSceen() {
    if (isMSAAOn)
        msaa->updateSceen(width, height);

    if (isHDROn)
        hdr->updateSceen(width, height);
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

    // HDR
    if (isHDROn && !hdr->enable)
        hdr->turnON(width, height);
    else if (!isHDROn && hdr->enable)
        hdr->turnOFF();
}