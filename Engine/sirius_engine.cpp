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
#include "shadow.h"

#include "Library/stb_image.h"

#include <iostream>
#include <vector>

GUI* gui;
CubeRenderer* cubeRenderer;
ModelRenderer* modelRenderer;
ModelRenderer* modelShadowRenderer;
SkyboxRenderer* skyboxRenderer;
MSAA* msaa;
Shadow* shadow;

SiriusEngine::SiriusEngine(GLFWwindow* _window, unsigned int _width,
                           unsigned int _height) :
    window(_window), width(_width), height(_height),
    camera(), dirLight(), keysPressed(), keysProcessed(),
    // ״̬��
    isDepthTestOn(true), isStencilTestOn(false), isFaceCullingOn(false),
    isMouseControlOn(true), isScrollControlOn(true), isShadowOn(false),
    isFreeLookingModeOn(false), isObjectRotationModeOn(false),
    isObjectCoordinateShown(true), isMSAAOn(false), isGammaOn(true),
    postProcessing(original),
    currentSelectedObjectIndex(-1),
    currentAddObjectIndex(0),
    currentSelectedPointLightIndex(-1),
    clearColor(0.1f, 0.1f, 0.1f) { 
    cubeVAO = 0; cubeVBO = 0;
}

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

    // ģ����ģ����Ӱ��Ⱦ
    Object* objectModel;
    objectModel = LoadPresets::loadModel(duck_model, u8"С��Ѽ");
    sceneObjects.push_back(objectModel);
    modelRenderer = new ModelRenderer(ResourceManager::getShader("model"));
    modelShadowRenderer = new ModelRenderer(ResourceManager::getShader("shadow"));

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

    // ��Ӱӳ��
    shadow = new Shadow();

    // gui
    gui = new GUI(*this);
    gui->initStyle();

    woodTexture = ResourceManager::getTexture("test");

    shader = ResourceManager::getShader("test");
    shader.setInt("diffuseTexture", 0, true);
    shader.setBool("shadowMap", 1);
}

void SiriusEngine::render() {
    this->configureRenderSetup();

    // MSAA֡��Ļ����ռ�����
    if (isMSAAOn)
        msaa->configureMSAASceenSetup();

    // ��Ӱӳ��
    if (isShadowOn) {
        // ��Ҫ���㶨���뾶
        dirLight.updatePosition();

        // ��ȡ�����붨���Ŀռ�����ת������
        glm::mat4 dirLightSpaceMatrix = dirLight.getLightSpaceMatrix();
        shadow->setLightSpaceMatrix(dirLightSpaceMatrix);

        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, shadow->depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture.ID);
        renderScene(shadow->shader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // set light uniforms
        shader.setVec3("viewPos", camera.position);
        shader.setVec3("lightPos", lightPos);
        shader.setMat4("lightSpaceMatrix", dirLight.getLightSpaceMatrix());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadow->depthMap);

        renderScene(shader);
    }



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


/*----------------------------------------------------------*/
void SiriusEngine::renderScene(Shader& shader) {
    // floor
    glm::mat4 model;
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shader.setMat4("model", model);
    renderCube();
}



void SiriusEngine::renderCube() {
    // initialize (if necessary)
    
    if (cubeVAO == 0) {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
             // bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             // top face
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

