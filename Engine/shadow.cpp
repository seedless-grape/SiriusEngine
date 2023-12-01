#include "shadow.h"
#include "Assets/resource_manager.h"

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

Shadow::Shadow() {
	// 配置阴影空间矩阵
	shader = ResourceManager::getShader("shadow");
	configureDepthFBO();

}

void Shadow::configureDepthFBO() {
	// 生成深度帧缓冲
	glGenFramebuffers(1, &depthMapFBO);

	// 生成绑定深度纹理
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	// 生成贴图，配置属性
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT,
				 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// 将深度贴图附在深度帧缓冲上
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
						   GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadow::setLightSpaceMatrix(glm::mat4 lightSpaceMatrix) {
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix, true);
}

