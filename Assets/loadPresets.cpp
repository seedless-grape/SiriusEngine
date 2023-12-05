#include "loadPresets.h"
#include "resource_manager.h"
#include "Core/light.h"

void LoadPresets::preLoad() {
	// ¼ÓÔØ×ÅÉ«Æ÷
	ResourceManager::loadShader("Shader/light_cube.vert", "Shader/light_cube.frag", nullptr, "light_cube");
	ResourceManager::loadShader("Shader/object.vert", "Shader/object.frag", nullptr, "object");
	ResourceManager::loadShader("Shader/model.vert", "Shader/model.frag", nullptr, "model");
	ResourceManager::loadShader("Shader/coordinate.vert", "Shader/coordinate.frag", nullptr, "coordinate");
	ResourceManager::loadShader("Shader/skybox.vert", "Shader/skybox.frag", nullptr, "skybox");
	ResourceManager::loadShader("Shader/msaa.vert", "Shader/msaa.frag", nullptr, "msaa");
	ResourceManager::loadShader("Shader/shadow.vert", "Shader/shadow.frag", nullptr, "shadow");
	ResourceManager::loadShader("Shader/hdr.vert", "Shader/hdr.frag", nullptr, "hdr");

	// ¼ÓÔØ²ÄÖÊ
	ResourceManager::loadTexture("Resources/textures/container2.png", "container_diffuse");
	ResourceManager::loadTexture("Resources/textures/container2_specular.png", "container_specular");
	ResourceManager::loadTexture("Resources/textures/brickwall.jpg", "test");


	std::vector<std::string> skyboxFaces {
		"Resources/skybox/cafe/right.jpg",
		"Resources/skybox/cafe/left.jpg",
		"Resources/skybox/cafe/top.jpg",
		"Resources/skybox/cafe/bottom.jpg",
		"Resources/skybox/cafe/front.jpg",
		"Resources/skybox/cafe/back.jpg"
	};
	ResourceManager::loadTexture(skyboxFaces, "skybox");
}

PointLight* LoadPresets::loadPointLight() {
	PointLight* pointLight = new PointLight();
	pointLight->enabled = true;
	return pointLight;
}

Object* LoadPresets::loadSkybox() {
	Object* skybox = new Skybox();
	skybox->diffuseTexture = ResourceManager::getTexture("skybox");
	return skybox;
}

Object* LoadPresets::loadCube(objectType type, std::string name) {
	Object* cube = nullptr;

	// ÔØÈë°××©·½¿é
	if (type == White_Box) {
		cube = new Cube(name);
		cube->position = glm::vec3(0.0f, 0.0f, -3.0f);

		cube->diffuseTexture = ResourceManager::getTexture("white_tiles");
		cube->specularTexture = ResourceManager::getTexture("pure_black");
		cube->shininess = 64.0f;
	}

	// ÔØÈëÄ¾Ïä
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

	// ÔØÈëÅèÔÔ
	if (type == plant_model) {
		//model = new Model(name, path + "duck/rubber_duck_toy_1k.obj");
		model = new Model(name, path + "potted_plant/potted_plant_04_2k.obj");

		
		model->position = glm::vec3(0.0f, 0.0f, -3.0f);
		model->shininess = 64.0f;
	}

	// ÔØÈë±³¾°°å
	if (type == background_model) {
		model = new Model(name, path + "background/background.obj");
		model->shininess = 64.0f;
	}

	// ÔØÈëÂíµñËÜ
	if (type == horse_model) {
		model = new Model(name, path + "horse/horse_statue_01_1k.obj");
		model->shininess = 64.0f;
	}

	return model;
}

