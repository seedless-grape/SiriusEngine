/* 光源 */

#ifndef LIGHT_H_
#define LIGHT_H_

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

// 渲染类前向声明
class Renderer;

// 基础光源类
static constexpr glm::vec3 LIGHT_POSITION(0.0f);
static constexpr glm::vec3 AMBIENT(0.1f);
static constexpr glm::vec3 DIFFUSE(0.6f);
static constexpr glm::vec3 SPECULAR(1.0f);

class Light {
public:
	// 光源属性
	glm::vec3 position;	// 光源位置坐标

	glm::vec3 ambient;  // 环境光照强度
	glm::vec3 diffuse;  // 漫反射光照强度
	glm::vec3 specular; // 镜面光照强度
	bool enabled;		// 启用该光源

public:
	Light(glm::vec3 _position = LIGHT_POSITION,
		  glm::vec3 _ambient = AMBIENT,
		  glm::vec3 _diffuse = DIFFUSE,
		  glm::vec3 _specular = SPECULAR);

	virtual ~Light() = default;

	// 光源绘制纯虚函数
	virtual void draw(Renderer& renderer, bool gamma = false) = 0;

	// 光源空间矩阵
	virtual glm::mat4 getLightSpaceMatrix() = 0;


	// 光源重置纯虚函数
	virtual void reset() = 0;
};

// 全局定向光照类
static constexpr glm::vec3 DIRECTION(0.0f, 0.0f, -1.0f);
static constexpr float DIR_RADIUS = 20.0f;

static constexpr float	DIR_RADIUS_MIN = 10.0f;		// 最小定向光半径
static constexpr float	DIR_RADIUS_MAX = 100.0f;	// 最大定向光半径

class DirLight : public Light {
public:
	glm::vec3 direction;	// 光照方向
	float radius;			// 半径(用于阴影计算)

public:
	DirLight(glm::vec3 _direction = DIRECTION,
			 glm::vec3 _ambient = AMBIENT,
			 glm::vec3 _diffuse = DIFFUSE,
			 glm::vec3 _specular = SPECULAR,
			 float _radius = DIR_RADIUS);

	DirLight(float x, float y, float z,
			 glm::vec3 _ambient = AMBIENT,
			 glm::vec3 _diffuse = DIFFUSE,
			 glm::vec3 _specular = SPECULAR,
			 float _radius = DIR_RADIUS);

	~DirLight() override = default;

	void draw(Renderer& renderer, bool gamma = false) override;

	glm::mat4 getLightSpaceMatrix() override;

	void updatePosition();

	void reset() override;
};

// 点光源类
static constexpr glm::vec3 POINT_LIGHT_POSITION(2.5f, 2.0f, -0.2f);
static constexpr glm::vec3 ATTENUATION_FACTORS(1.0f, 0.0f, 0.0f);

class PointLight : public Light {
public:
	// 着色点与光源距离光照强度衰减系数
	glm::vec3 attenuationFactors;
	//float constant; // 常数项
	//float linear; // 一次项
	//float quadratic; // 二次项

public:
	PointLight(glm::vec3 _position = POINT_LIGHT_POSITION,
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

	void draw(Renderer& renderer, bool gamma = false) override;

	glm::mat4 getLightSpaceMatrix() override;

	void reset() override;
};

// 聚光类
class SpotLight : public PointLight {
public:
	glm::vec3 direction; // 聚光光照方向

	// 聚光属性
	float inCutOff; // 聚光半径内切光角
	float outCutOff; // 聚光半径外切光角

public:
	SpotLight(glm::vec3 _position, glm::vec3 _direcion);

	void setCutOff(float inCutOff, float outCutOff);

	~SpotLight() override = default;
};

#endif