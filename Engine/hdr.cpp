#include "hdr.h"
#include "Assets/resource_manager.h"

#include <iostream>

HDR::HDR() : enable(false), quadVAO(0) {}

void HDR::turnON(unsigned int width, unsigned int height) {
    this->enable = true;
    this->quadVAO = 0;
    this->HDRwidth = width;
    this->HDRheight = height;
    this->colorBuffer = configureHDRColorBuffer();
    this->HDRFBO = configureHDRFBO();

    this->shader = ResourceManager::getShader("hdr");
    configureShader();
}

void HDR::turnOFF() {
    this->enable = false;
    glDeleteVertexArrays(1, &this->quadVAO);
    glDeleteFramebuffers(1, &this->HDRFBO);
    glDeleteTextures(1, &this->colorBuffer);
}

void HDR::updateSceen(unsigned int width, unsigned int height) {
    turnOFF();
    turnON(width, height);
}

void HDR::configureHDRSceenSetup() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->HDRFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void HDR::render() {
    // HDR帧缓冲渲染
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    shader.setInt("hdrBuffer", 0, true);
    shader.setFloat("exposure", 1.5f);
    renderQuad();
}

unsigned int HDR::configureHDRFBO() {
    // 生成FBO
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);

    // 创建深度缓冲
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->HDRwidth, this->HDRheight);

    // 链接颜色缓冲、深度缓冲附件
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    // 解绑
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return FBO;
}

unsigned int HDR::configureHDRColorBuffer() {
    // 生成颜色缓冲
    unsigned int buffer;
    glGenTextures(1, &buffer);
    glBindTexture(GL_TEXTURE_2D, buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->HDRwidth, this->HDRheight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return buffer;
}

void HDR::configureShader() {
    this->shader.setInt("hdrBuffer", 0, true);
}


void HDR::renderQuad() {
    unsigned int quadVBO;
    if (quadVAO == 0) {
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        // 设置HDR空间VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}