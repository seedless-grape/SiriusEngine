/* 渲染管线 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"

// 前向声明
class Object;
class DirLight;
class PointLight;

// 渲染后期特效属性
enum PostProcessing {
	original, inverse, grayscale, sharpen, blur, edgeDetection, postProcessingCount
};

// 基础渲染类
class Renderer {
public:
	Renderer(const Shader& shader);

	virtual ~Renderer() = default;


	// 渲染
	virtual void render(const Object& object, bool drawCoordinate = true, bool gamma = false) = 0;


	virtual void render(const PointLight& pointLight, bool gamma = false);

	// 更新渲染器
	void updateRenderer(glm::mat4 spaceMatrix, glm::vec3 viewPos,
						const DirLight& dirLight,
						const std::vector<PointLight*>& pointLights);

public:
	// 后期特效
	int postProcessing;


	Shader objectShader;

protected:
	
	Shader coordinateShader;
	Shader lightCubeShader;
	unsigned int coordinateVAO;

	virtual void initRenderData() {};

	// 坐标绘制数据
	void initCoordinateRenderData();

	// 绘制坐标
	void renderCoordinate();
};

#endif // !RENDERER_H_
