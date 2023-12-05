/*
shadow������ʵ��������Ӱӳ��
	Ϊÿ����Դ����һ�������ͼ����������ռ�����Ⱦ����ͼ
	�Ƚ���ȴ��������ͼ������λ��Ϊ��Ӱλ��
*/

#ifndef SHADOW_H_
#define SHADOW_H_

#include "Core/shader.h"
#include "Core/renderer.h"

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

// ��Ӱ��
class Shadow {
public:
	bool isShadowOn;	// �Ƿ�����Ӱ
	bool isBias;		// �Ƿ���ƫ���Ż�
	bool isCull;		// �Ƿ��������޳�
	bool isSoft;		// �Ƿ���PCF����Ӱ

public:
	// ����/��������
	Shadow(unsigned int _width = SHADOW_WIDTH,
		   unsigned int _height = SHADOW_HEIGHT);

	~Shadow() = default;

	// ��ȡ��Ӱ��ͼID
	unsigned int getDepthMap();

	// ���ù�Դ�ռ�λ��
	void setLightSpaceMatrix(glm::mat4 lightSpaceMatrix);

	// ����Ӱ֡������
	void bindShadowFBO();

	// �ָ�Ĭ��֡������
	void unbindShadowFBO(unsigned int FBO, unsigned int width, unsigned int height);

private:
	unsigned int width;
	unsigned int height;

	Shader shader;				// �����Ϣ��ɫ��
	unsigned int depthMapFBO;	// ��Ӱ��ͼ֡������
	unsigned int depthMap;		// ��Ӱ��ͼ

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
