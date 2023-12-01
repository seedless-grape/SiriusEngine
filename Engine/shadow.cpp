#include "shadow.h"
#include "Assets/resource_manager.h"

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

Shadow::Shadow() {
	// ������Ӱ�ռ����
	shader = ResourceManager::getShader("shadow");
	configureDepthFBO();

}

void Shadow::configureDepthFBO() {
	// �������֡����
	glGenFramebuffers(1, &depthMapFBO);

	// ���ɰ��������
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	// ������ͼ����������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT,
				 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// �������ͼ�������֡������
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

