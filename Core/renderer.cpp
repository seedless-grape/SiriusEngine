#include "renderer.h"
#include "Assets/resource_manager.h"
#include "light.h"
#include "object.h"
#include <sstream>
#include <iostream>

// ����������Ⱦ
Renderer::Renderer(const Shader& shader) :
    coordinateVAO(0), postProcessing(original) {
    this->objectShader = shader;

    // ����������ɫ
    this->coordinateShader = ResourceManager::getShader("coordinate");

    // ������Դ��������ɫ
    this->lightCubeShader = ResourceManager::getShader("light_cube");

    this->initCoordinateRenderData();
}

void Renderer::render(const PointLight& pointLight, bool gamma) {}

void Renderer::updateRenderer(glm::mat4 spaceMatrix,
                              glm::vec3 viewPos,
                              const DirLight& dirLight,
                              const std::vector<PointLight*>& pointLights) {
    // ������ɫ
    this->objectShader.setMat4("spaceMatrix", spaceMatrix, true);
    this->objectShader.setVec3("viewPos", viewPos);

    // ȫ�ֶ������
    this->objectShader.setVec3("dirLight.direction", dirLight.direction);
    this->objectShader.setVec3("dirLight.ambient", dirLight.ambient);
    this->objectShader.setVec3("dirLight.diffuse", dirLight.diffuse);
    this->objectShader.setVec3("dirLight.specular", dirLight.specular);
    this->objectShader.setBool("dirLight.enabled", dirLight.enabled);

    // ���Դ����
    this->objectShader.setInt("pointLightsNum", pointLights.size());

    for (int i = 0; i < pointLights.size(); i++) {
        std::stringstream sstr;
        sstr << "pointLights[" << (char)(i + '0') << "]";
        this->objectShader.setVec3((sstr.str() + ".position").c_str(), pointLights[i]->position);
        this->objectShader.setVec3((sstr.str() + ".ambient").c_str(), pointLights[i]->ambient);
        this->objectShader.setVec3((sstr.str() + ".diffuse").c_str(), pointLights[i]->diffuse);
        this->objectShader.setVec3((sstr.str() + ".specular").c_str(), pointLights[i]->specular);
        this->objectShader.setFloat((sstr.str() + ".constant").c_str(), pointLights[i]->attenuationFactors.x);
        this->objectShader.setFloat((sstr.str() + ".linear").c_str(), pointLights[i]->attenuationFactors.y);
        this->objectShader.setFloat((sstr.str() + ".quadratic").c_str(), pointLights[i]->attenuationFactors.z);
        this->objectShader.setBool((sstr.str() + ".enabled").c_str(), pointLights[i]->enabled);
    }

    // ���������Ч
    this->objectShader.setInt("postProcessing", postProcessing);
    
    // ��������ɫ
    this->coordinateShader.setMat4("spaceMatrix", spaceMatrix, true);

    // ��Դ��������ɫ
    this->lightCubeShader.setMat4("spaceMatrix", spaceMatrix, true);

    // ��Դ�����������Ч
    this->lightCubeShader.setInt("postProcessing", postProcessing);

    
}

void Renderer::initCoordinateRenderData() {
    float vertices[] = {
        // position,      color
        // x-axis (red)
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // y-axis (green)
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        // z-axis (blue)
        0.0f, 0.0f, 0.0f, 0.0f, 0.36f, 0.95f,
        0.0f, 0.0f, 0.5f, 0.0f, 0.36f, 0.95f
    };

    // ����VAO��VBO
    unsigned int VBO;
    glGenVertexArrays(1, &this->coordinateVAO);
    glGenBuffers(1, &VBO);

    // �������뻺����
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ������������
    glBindVertexArray(this->coordinateVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    // ���
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::renderCoordinate() {
    // ��������ʱ�ر���Ȳ���
    glDisable(GL_DEPTH_TEST);

    this->coordinateShader.use();
    glBindVertexArray(this->coordinateVAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}
