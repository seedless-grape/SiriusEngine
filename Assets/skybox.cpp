#include <iostream>
#include "skybox.h"

// 天空盒
Skybox::Skybox(std::string _name) : Object(_name) {}

void Skybox::draw(Renderer& renderer, Shadow* shadow, bool drawCoordinate, bool gamma) {
    renderer.render(*this, drawCoordinate, gamma);
}

// 天空盒渲染
SkyboxRenderer::SkyboxRenderer(const Shader& shader) :
    Renderer(shader), skyboxVAO(0) {
    this->initRenderData();
}

SkyboxRenderer::~SkyboxRenderer() {
    glDeleteVertexArrays(1, &this->skyboxVAO);
    glDeleteVertexArrays(1, &this->coordinateVAO);
}

void SkyboxRenderer::render(const Object& object, bool drawCoordinate, bool gamma) {
    glDepthFunc(GL_LEQUAL);
    this->objectShader.use();

    this->objectShader.setInt("skybox", 0);
    this->objectShader.setBool("gamma", gamma);

    // 纹理映射
    glBindVertexArray(this->skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    object.diffuseTexture.bind(GL_TEXTURE_CUBE_MAP);

    // 图形绘制
    glBindVertexArray(this->skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void SkyboxRenderer::updateRenderer(glm::mat4 spaceMatrix) {
    // 着色
    this->objectShader.setMat4("spaceMatrix", spaceMatrix, true);
}

void SkyboxRenderer::initRenderData() {
    float skyboxVertices[] = {
        // 顶点坐标          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // 创建VAO和VBO
    unsigned int VBO;
    glGenVertexArrays(1, &this->skyboxVAO);
    glGenBuffers(1, &VBO);

    // 数据流入缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    // 解释数据属性
    glBindVertexArray(this->skyboxVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 解绑
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}