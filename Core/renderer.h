/* ��Ⱦ���� */

#ifndef RENDERER_H_
#define RENDERER_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"

// ǰ������
class Object;
class DirLight;
class PointLight;

// ������Ⱦ��
class Renderer {
public:
	Renderer(const Shader& shader);

	virtual ~Renderer() = default;

	// ��Ⱦ
	virtual void render(const Object& object, bool drawCoordinate = true) = 0;

	virtual void render(const PointLight& pointLight) = 0;

	// ������Ⱦ��
	void updateRenderer(glm::mat4 spaceMatrix, glm::vec3 viewPos,
						const DirLight& dirLight,
						const std::vector<PointLight*>& pointLights);

protected:
	Shader objectShader;
	Shader coordinateShader;
	Shader lightCubeShader;
	unsigned int coordinateVAO;

	virtual void initRenderData() = 0;

	// �����������
	void initCoordinateRenderData();

	// ��������
	void renderCoordinate();
};

#endif // !RENDERER_H_
