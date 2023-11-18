#include "light.h"
#include "renderer.h"

// 基础光源

Light::Light(glm::vec3 _ambient, glm::vec3 _diffuse,
			 glm::vec3 _specular) :
	ambient(_ambient), diffuse(_diffuse), specular(_specular),
	enabled(true) {}

// 全局定向光照

DirLight::DirLight(glm::vec3 _direction, glm::vec3 _ambient,
				   glm::vec3 _diffuse, glm::vec3 _specular) :
	Light(_ambient, _diffuse, _specular), direction(_direction) {}

DirLight::DirLight(float x, float y, float z,
				   glm::vec3 _ambient, glm::vec3 _diffuse,
				   glm::vec3 _specular) :
	Light(_ambient, _diffuse, _specular), direction(x, y, z) {}

void DirLight::draw(Renderer& renderer) {}

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
	Light(_ambient, _diffuse, _specular), position(_position),
	attenuationFactors(_attenuationFactors) {}

PointLight::PointLight(float x, float y, float z,
					   glm::vec3 _ambient,
					   glm::vec3 _diffuse,
					   glm::vec3 _specular,
					   glm::vec3 _attenuationFactors) :
	Light(_ambient, _diffuse, _specular), position(x, y, z),
	attenuationFactors(_attenuationFactors) {}

void PointLight::draw(Renderer& renderer) {
	renderer.render(*this);
}

void PointLight::reset() {
	position = LIGHT_POSITION;
	ambient = AMBIENT;
	diffuse = DIFFUSE;
	specular = SPECULAR;
	attenuationFactors = ATTENUATION_FACTORS;
}
