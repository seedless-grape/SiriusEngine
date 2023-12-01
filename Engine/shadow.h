/*
shadow������ʵ��������Ӱӳ��
	Ϊÿ����Դ����һ�������ͼ����������ռ�����Ⱦ����ͼ
	�Ƚ���ȴ��������ͼ������λ��Ϊ��Ӱλ��
*/


#ifndef SHADOW_H_
#define SHADOW_H_

#include "Core/shader.h"

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
	Shadow();

	~Shadow() = default;

	// ���ù�Դ�ռ�λ��
	void setLightSpaceMatrix(glm::mat4 lightSpaceMatrix);


private:


private:
	// ���������ͼ֡������
	void configureDepthFBO();

};


#endif // !SHADOW_H_

