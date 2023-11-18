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

// 基础渲染类
class Renderer {
public:
	Renderer(const Shader& shader);

	virtual ~Renderer() = default;

	// 渲染
	virtual void render(const Object& object, bool drawCoordinate = true) = 0;

	virtual void render(const PointLight& pointLight) = 0;

	// 更新渲染器
	void updateRenderer(glm::mat4 spaceMatrix, glm::vec3 viewPos,
						const DirLight& dirLight,
						const std::vector<PointLight*>& pointLights);

protected:
	Shader objectShader;
	Shader coordinateShader;
	Shader lightCubeShader;
	unsigned int coordinateVAO;

	virtual void initRenderData() = 0;

	// 坐标绘制数据
	void initCoordinateRenderData();

	// 绘制坐标
	void renderCoordinate();
};

#endif // !RENDERER_H_
