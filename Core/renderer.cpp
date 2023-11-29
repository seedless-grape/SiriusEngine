#include "renderer.h"
#include "Assets/resource_manager.h"
#include "light.h"
#include "object.h"
#include <sstream>
#include <iostream>

// 基础场景渲染
Renderer::Renderer(const Shader& shader) :
    coordinateVAO(0), postProcessing(original) {
    this->objectShader = shader;

    // 创建坐标着色
    this->coordinateShader = ResourceManager::getShader("coordinate");

    // 创建光源立方体着色
    this->lightCubeShader = ResourceManager::getShader("light_cube");

    this->initCoordinateRenderData();
}

void Renderer::render(const PointLight& pointLight, bool gamma) {}

void Renderer::updateRenderer(glm::mat4 spaceMatrix,
                              glm::vec3 viewPos,
                              const DirLight& dirLight,
                              const std::vector<PointLight*>& pointLights) {
    // 物体着色
    this->objectShader.setMat4("spaceMatrix", spaceMatrix, true);
    this->objectShader.setVec3("viewPos", viewPos);

    // 全局定向光照
    this->objectShader.setVec3("dirLight.direction", dirLight.direction);
    this->objectShader.setVec3("dirLight.ambient", dirLight.ambient);
    this->objectShader.setVec3("dirLight.diffuse", dirLight.diffuse);
    this->objectShader.setVec3("dirLight.specular", dirLight.specular);
    this->objectShader.setBool("dirLight.enabled", dirLight.enabled);

    // 点光源光照
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

    // 物体后期特效
    this->objectShader.setInt("postProcessing", postProcessing);
    
    // 坐标轴着色
    this->coordinateShader.setMat4("spaceMatrix", spaceMatrix, true);

    // 光源立方体着色
    this->lightCubeShader.setMat4("spaceMatrix", spaceMatrix, true);

    // 光源立方体后期特效
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

    // 创建VAO和VBO
    unsigned int VBO;
    glGenVertexArrays(1, &this->coordinateVAO);
    glGenBuffers(1, &VBO);

    // 数据流入缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 解释数据属性
    glBindVertexArray(this->coordinateVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    // 解绑
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::renderCoordinate() {
    // 绘制坐标时关闭深度测试
    glDisable(GL_DEPTH_TEST);

    this->coordinateShader.use();
    glBindVertexArray(this->coordinateVAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}
