/* ��Դ */

#ifndef LIGHT_H_
#define LIGHT_H_

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

// ��Ⱦ��ǰ������
class Renderer;

// ������Դ��
const glm::vec3 AMBIENT(0.1f);
const glm::vec3 DIFFUSE(0.6f);
const glm::vec3 SPECULAR(1.0f);

class Light {
public:
	// ��Դ����
	glm::vec3 ambient;  // ��������ǿ��
	glm::vec3 diffuse;  // ���������ǿ��
	glm::vec3 specular; // �������ǿ��
	bool enabled;		// ���øù�Դ

public:
	Light(glm::vec3 _ambient = AMBIENT,
		  glm::vec3 _diffuse = DIFFUSE,
		  glm::vec3 _specular = SPECULAR);

	virtual ~Light() = default;

	// ��Դ���ƴ��麯��
	virtual void draw(Renderer& renderer) = 0;

	// ��Դ���ô��麯��
	virtual void reset() = 0;
};

// ȫ�ֶ��������
const glm::vec3 DIRECTION(0.0f, 0.0f, -1.0f);

class DirLight : public Light {
public:
	glm::vec3 direction; // ���շ���

public:
	DirLight(glm::vec3 _direction = DIRECTION,
			 glm::vec3 _ambient = AMBIENT,
			 glm::vec3 _diffuse = DIFFUSE,
			 glm::vec3 _specular = SPECULAR);

	DirLight(float x, float y, float z,
			 glm::vec3 _ambient = AMBIENT,
			 glm::vec3 _diffuse = DIFFUSE,
			 glm::vec3 _specular = SPECULAR);

	~DirLight() override = default;

	void draw(Renderer& renderer) override;

	void reset() override;
};

// ���Դ��
const glm::vec3 LIGHT_POSITION(2.5f, 2.0f, -0.2f);
const glm::vec3 ATTENUATION_FACTORS(1.0f, 0.0f, 0.0f);

class PointLight : public Light {
public:
	glm::vec3 position; // ���Դ����

	// ��ɫ�����Դ�������ǿ��˥��ϵ��
	glm::vec3 attenuationFactors;
	//float constant; // ������
	//float linear; // һ����
	//float quadratic; // ������

public:
	PointLight(glm::vec3 _position = LIGHT_POSITION,
			   glm::vec3 _ambient = AMBIENT,
			   glm::vec3 _diffuse = DIFFUSE,
			   glm::vec3 _specular = SPECULAR,
			   glm::vec3 _attenuationFactors = ATTENUATION_FACTORS);

	PointLight(float x, float y, float z,
			   glm::vec3 _ambient = AMBIENT,
			   glm::vec3 _diffuse = DIFFUSE,
			   glm::vec3 _specular = SPECULAR,
			   glm::vec3 _attenuationFactors = ATTENUATION_FACTORS);

	~PointLight() override = default;

	void draw(Renderer& renderer) override;

	void reset() override;
};

// �۹���
class SpotLight : public PointLight {
public:
	glm::vec3 direction; // �۹���շ���

	// �۹�����
	float inCutOff; // �۹�뾶���й��
	float outCutOff; // �۹�뾶���й��

public:
	SpotLight(glm::vec3 _position, glm::vec3 _direcion);

	void setCutOff(float inCutOff, float outCutOff);

	~SpotLight() override = default;
};

#endif