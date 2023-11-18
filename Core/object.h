/* 物体 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

#include "texture.h"

// 渲染前向声明
class Renderer;

// 基础物体类
class Object {
public:
	// 物体属性
	glm::vec3 position;	// 位置坐标
	glm::vec3 rotation;	// 旋转角度
	glm::vec3 scale;	// 缩放比例
	glm::vec3 color;	// 物体颜色
	std::string name;	// 物体名称
	bool enabled;
	bool selected;

	// 物体纹理
	Texture diffuseTexture;		// 漫反射材质
	Texture specularTexture;	// 镜面反射材质
	float shininess;			// 散光度

public:
	// 构造函数
	Object(std::string _name = "Object");

	Object(glm::vec3 _position,
		   glm::vec3 _rotation = glm::vec3(0.0f),
		   glm::vec3 _scale = glm::vec3(1.0f),
		   glm::vec3 _color = glm::vec3(1.0f),
		   std::string _name = "Object");

	// 析构函数
	virtual ~Object() = default;

	// 物体绘制(纯虚函数)
	virtual void draw(Renderer& renderer, bool drawCoordinate = true) = 0;

	// 重置物体
	void reset();
};

#endif // !OBJECT_H_
