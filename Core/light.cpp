#include "light.h"
#include "renderer.h"

// 基础光源

Light::Light(glm::vec3 _position, glm::vec3 _ambient,
			 glm::vec3 _diffuse, glm::vec3 _specular) :
	position(_position), enabled(true),
	ambient(_ambient), diffuse(_diffuse), specular(_specular) {}

// 全局定向光照

DirLight::DirLight(glm::vec3 _direction, glm::vec3 _ambient,
				   glm::vec3 _diffuse, glm::vec3 _specular, float _radius) :
	Light(_ambient, _diffuse, _specular), direction(_direction), radius(_radius) {}

DirLight::DirLight(float x, float y, float z,
				   glm::vec3 _ambient, glm::vec3 _diffuse,
				   glm::vec3 _specular, float _radius) :
	Light(_ambient, _diffuse, _specular), direction(x, y, z), radius(_radius) {}

void DirLight::draw(Renderer& renderer, bool gamma) {}

glm::mat4 DirLight::getLightSpaceMatrix() {
	glm::mat4 dirLightProjection, dirLightView;
	glm::mat4 dirLightSpaceMatrix;

	float near_plane = 1.0f, far_plane = 2 * radius;
	dirLightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	dirLightView = glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	dirLightSpaceMatrix = dirLightProjection * dirLightView;

	return dirLightSpaceMatrix;
}

void DirLight::updatePosition() {
	position = radius * glm::normalize(direction);
}

void DirLight::reset() {
	direction = DIRECTION;
	ambient = AMBIENT;
	diffuse = DIFFUSE;
	specular = SPECULAR;
}

// 点光源

PointLight::PointLight(glm::vec3 _position,
					   glm::vec3 _ambient,
					   glm::vec3 _diffuse,
					   glm::vec3 _specular,
					   glm::vec3 _attenuationFactors) :
	Light(_position, _ambient, _diffuse, _specular),
	attenuationFactors(_attenuationFactors) {}

PointLight::PointLight(float x, float y, float z,
					   glm::vec3 _ambient,
					   glm::vec3 _diffuse,
					   glm::vec3 _specular,
					   glm::vec3 _attenuationFactors) :
	Light(glm::vec3(x, y, z), _ambient, _diffuse, _specular),
	attenuationFactors(_attenuationFactors) {}

void PointLight::draw(Renderer& renderer, bool gamma) {
	renderer.render(*this, gamma);
}

glm::mat4 PointLight::getLightSpaceMatrix() {
	return glm::mat4(0.0f);
}

void PointLight::reset() {
	position = LIGHT_POSITION;
	ambient = AMBIENT;
	diffuse = DIFFUSE;
	specular = SPECULAR;
	attenuationFactors = ATTENUATION_FACTORS;
}
