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

// ��Ⱦ������Ч����
enum PostProcessing {
	original, inverse, grayscale, sharpen, blur, edgeDetection, postProcessingCount
};

// ������Ⱦ��
class Renderer {
public:
	Renderer(const Shader& shader);

	virtual ~Renderer() = default;

	// ��Ⱦ
	virtual void render(const Object& object, bool drawCoordinate = true, bool gamma = false) = 0;

	virtual void render(const PointLight& pointLight, bool gamma = false);

	// ������Ⱦ��
	void updateRenderer(glm::mat4 spaceMatrix, glm::vec3 viewPos,
						const DirLight& dirLight,
						const std::vector<PointLight*>& pointLights);

public:
	// ������Ч
	int postProcessing;
	Shader objectShader;

protected:
	
	Shader coordinateShader;
	Shader lightCubeShader;
	unsigned int coordinateVAO;

	virtual void initRenderData() {};

	// �����������
	void initCoordinateRenderData();

	// ��������
	void renderCoordinate();
};

#endif // !RENDERER_H_
