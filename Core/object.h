/* ���� */

#ifndef OBJECT_H_
#define OBJECT_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

#include "texture.h"

// ��Ⱦǰ������
class Renderer;

// ����������
class Object {
public:
	// ��������
	glm::vec3 position;	// λ������
	glm::vec3 rotation;	// ��ת�Ƕ�
	glm::vec3 scale;	// ���ű���
	glm::vec3 color;	// ������ɫ
	std::string name;	// ��������
	bool enabled;
	bool selected;

	// ��������
	Texture diffuseTexture;		// ���������
	Texture specularTexture;	// ���淴�����
	float shininess;			// ɢ���

public:
	// ���캯��
	Object(std::string _name = "Object");

	Object(glm::vec3 _position,
		   glm::vec3 _rotation = glm::vec3(0.0f),
		   glm::vec3 _scale = glm::vec3(1.0f),
		   glm::vec3 _color = glm::vec3(1.0f),
		   std::string _name = "Object");

	// ��������
	virtual ~Object() = default;

	// �������(���麯��)
	virtual void draw(Renderer& renderer, bool drawCoordinate = true) = 0;

	// ��������
	void reset();
};

#endif // !OBJECT_H_
