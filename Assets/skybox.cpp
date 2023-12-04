#include <iostream>
#include "skybox.h"

// ��պ�
Skybox::Skybox(std::string _name) : Object(_name) {}

void Skybox::draw(Renderer& renderer, Shadow* shadow, bool drawCoordinate, bool gamma) {
    renderer.render(*this, drawCoordinate, gamma);
}

// ��պ���Ⱦ
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

    // ����ӳ��
    glBindVertexArray(this->skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    object.diffuseTexture.bind(GL_TEXTURE_CUBE_MAP);

    // ͼ�λ���
    glBindVertexArray(this->skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void SkyboxRenderer::updateRenderer(glm::mat4 spaceMatrix) {
    // ��ɫ
    this->objectShader.setMat4("spaceMatrix", spaceMatrix, true);
}

void SkyboxRenderer::initRenderData() {
    float skyboxVertices[] = {
        // ��������          
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

    // ����VAO��VBO
    unsigned int VBO;
    glGenVertexArrays(1, &this->skyboxVAO);
    glGenBuffers(1, &VBO);

    // �������뻺����
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    // ������������
    glBindVertexArray(this->skyboxVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // ���
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}