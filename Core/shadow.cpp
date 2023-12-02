#include "shadow.h"
#include "Assets/resource_manager.h"
#include <iostream>

Shadow::Shadow(unsigned int _width, unsigned int _height) :
	width(_width), height(_height) {
	// 配置阴影空间矩阵
	shader = ResourceManager::getShader("shadow");
	configureDepthFBO();

}

void Shadow::setLightSpaceMatrix(glm::mat4 lightSpaceMatrix) {
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix, true);
}

void Shadow::bindShadowFBO() {
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Shadow::unbindShadowFBO(unsigned int FBO, unsigned int width, unsigned int height) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Shadow::configureDepthFBO() {
	// 生成深度帧缓冲
	glGenFramebuffers(1, &depthMapFBO);

	// 生成绑定深度纹理
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	// 生成贴图，配置属性
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
				 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// 将深度贴图附在深度帧缓冲上
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
						   GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// 解绑
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowRenderer::ShadowRenderer(const Shader& shader) : Renderer(shader) {}

void ShadowRenderer::render(const Object& object, bool drawCoordinate, bool gamma) {
	this->objectShader.use();
	glm::mat4 modelMatrix;
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, object.position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotation.x),
							  glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotation.z),
							  glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotation.y),
							  glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, object.scale);

	this->objectShader.setMat4("modelMatrix", modelMatrix);
}

