#include "object.h"
#include "renderer.h"

// 基础物体类

Object::Object(std::string _name) :
	name(_name), position(0.0f), rotation(0.0f), scale(1.0f),
	color(1.0f), enabled(true), selected(false),
	diffuseTexture(), specularTexture(), shininess(32.0f) {}

Object::Object(glm::vec3 _position, glm::vec3 _rotation,
			   glm::vec3 _scale, glm::vec3 _color,
			   std::string _name) :
	name(_name), position(_position), rotation(_rotation),
	scale(_scale), color(_color), enabled(true), selected(false),
	diffuseTexture(), specularTexture(), shininess(32.0f) {}

void Object::reset() {
	position = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);
	color = glm::vec3(1.0f);
	enabled = true;
	selected = false;
	shininess = 32.0f;
}
