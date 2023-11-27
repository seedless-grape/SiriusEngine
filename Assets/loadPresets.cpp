#include "loadPresets.h"
#include "resource_manager.h"
#include "Core/light.h"

PointLight* LoadPresets::loadPointLight() {
	PointLight* pointLight = new PointLight();
	pointLight->enabled = true;
	return pointLight;
}

Object* LoadPresets::loadCube(objectType type, std::string name) {
	Object* cube = nullptr;

	// �����ש����
	if (type == White_Box) {
		cube = new Cube(name);
		cube->position = glm::vec3(0.0f, 0.0f, -3.0f);

		cube->diffuseTexture = ResourceManager::getTexture("white_tiles");
		cube->specularTexture = ResourceManager::getTexture("pure_black");
		cube->shininess = 64.0f;
	}

	// ����ľ��
	if (type == Wooden_Box) {
		cube = new Cube(name);
		cube->position = glm::vec3(0.0f, 0.0f, -3.0f);
		cube->diffuseTexture = ResourceManager::getTexture("container_diffuse");
		cube->specularTexture = ResourceManager::getTexture("container_specular");
		cube->shininess = 64.0f;
	}
	
	return cube;
}

Object* LoadPresets::loadModel(modelType type, std::string name) {
	Object* model = nullptr;
	std::string path = "Resources/model/";

	// ����С��Ѽ
	if (type == duck_model) {
		model = new Model(name, path + "duck/rubber_duck_toy_1k.obj");
		model->position = glm::vec3(0.0f, 0.0f, -3.0f);
		model->shininess = 64.0f;
	}

	return model;
}

