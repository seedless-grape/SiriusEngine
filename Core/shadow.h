/*
shadow������ʵ��������Ӱӳ��
	Ϊÿ����Դ����һ�������ͼ����������ռ�����Ⱦ����ͼ
	�Ƚ���ȴ��������ͼ������λ��Ϊ��Ӱλ��
*/


#ifndef SHADOW_H_
#define SHADOW_H_

#include "Core/shader.h"
#include "Core/renderer.h"

const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

// ��Ӱ��
class Shadow {
public:
	Shader shader;				// �����Ϣ��ɫ��
	unsigned int depthMapFBO;	// ��Ӱ��ͼ֡������
	unsigned int depthMap;		// ��Ӱ��ͼ

	unsigned int width;
	unsigned int height;

public:
	// ����/��������
	Shadow(unsigned int _width = SHADOW_WIDTH,
		   unsigned int _height = SHADOW_HEIGHT);

	~Shadow() = default;

	// ���ù�Դ�ռ�λ��
	void setLightSpaceMatrix(glm::mat4 lightSpaceMatrix);

	// ����Ӱ֡������
	void bindShadowFBO();

	// �ָ�Ĭ��֡������
	void unbindShadowFBO(unsigned int FBO, unsigned int width, unsigned int height);

private:


private:
	// ���������ͼ֡������
	void configureDepthFBO();

};

class ShadowRenderer : public Renderer {
public:
	// ����/��������
	ShadowRenderer(const Shader& shader);

	~ShadowRenderer() override = default;

	// ��Ⱦ
	void render(const Object& object, bool drawCoordinate = false, bool gamma = false) override;
};


#endif // !SHADOW_H_

