#include "msaa.h"
#include "Assets/resource_manager.h"

#include <iostream>

// ��������

MSAA::MSAA() : enable(false) {}

void MSAA::turnON(unsigned int width, unsigned int height) {
    this->enable = true;
    this->MSAAwidth = width;
    this->MSAAheight = height;
    this->VAO = setupVAO();
    this->screenTexture = configureSceenTexture();
    this->MSAAFBO = configureMSAAFBO();  
    this->intermediateFBO = configureIntermediateFBO();

    if (ResourceManager::hasShader("msaa"))
        this->shader = ResourceManager::getShader("msaa");
    else
        this->shader = ResourceManager::loadShader("Shader/msaa.vert",
                                                   "Shader/msaa.frag",
                                                   nullptr, "msaa");

    configureShader();
}

void MSAA::turnOFF() {
    this->enable = false;
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteFramebuffers(1, &this->MSAAFBO);
    glDeleteFramebuffers(1, &this->intermediateFBO);
    glDeleteTextures(1, &this->screenTexture);
}

void MSAA::updateSceen(unsigned int width, unsigned int height) {
    turnOFF();
    turnON(width, height);
}

void MSAA::configureMSAASceenSetup() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSAAFBO);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void MSAA::render() {
    // 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSAAFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
    glBlitFramebuffer(0, 0, this->MSAAwidth, this->MSAAheight, 0, 0,
                      this->MSAAwidth, this->MSAAheight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // 3. now render quad with scene's visuals as its texture image
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // draw Screen quad
    this->shader.use();
    glBindVertexArray(this->VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->screenTexture); // use the now resolved color attachment as the quad's texture
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// ˽�к���

unsigned int MSAA::setupVAO() {
    unsigned int quadVAO, quadVBO;

    // �ڹ�һ���豸���������������Ļ���ı��εĶ�������
    float quadVertices[] = {
        // λ������    // ��������
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    return quadVAO;
}

unsigned int MSAA::configureMSAAFBO() {
    // ����MSAA֡����ռ�
    unsigned int MSAAFramebuffer;
    glGenFramebuffers(1, &MSAAFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, MSAAFramebuffer);

    // ���ɰ�MSAA��ɫ��������
    unsigned int textureColorBufferMultiSampled;
    glGenTextures(1, &textureColorBufferMultiSampled);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);   // ���ز���
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB,
                            this->MSAAwidth, this->MSAAheight, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D_MULTISAMPLE,
                           textureColorBufferMultiSampled, 0);

    // ���ɰ�MSAA���/ģ����Ⱦ����ռ�
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8,
                                     this->MSAAwidth, this->MSAAheight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // ֡����ռ��쳣���
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    // ���MSAA֡����ռ�
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return MSAAFramebuffer;
}

unsigned int MSAA::configureIntermediateFBO() {
    // �����м����֡������
    unsigned int intermediateFBO;
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

    // ֻ��Ҫ��ɫ����ֵ
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

    // �м仺��ռ��쳣���
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;

    // ���
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return intermediateFBO;
}

// ���ɰ��м仺������ɫ��������
unsigned int MSAA::configureSceenTexture() {
    unsigned texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->MSAAwidth, this->MSAAheight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}

void MSAA::configureShader() {
    shader.use();
    shader.setInt("screenTexture", 0);
}
