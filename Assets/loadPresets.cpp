#include "loadPresets.h"
#include "resource_manager.h"
#include "Core/light.h"

void LoadPresets::preLoadShaderTexture() {
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

	switch (type) {
		case plant_model:
			// ÅèÔÔ
			model = new Model(name, path + "potted_plant/potted_plant_04_2k.obj");
			break;
		case background_model:
			// ±³¾°°å
			model = new Model(name, path + "background/background.obj");
			break;
		case horse_model:
			// ÂíµñËÜ
			model = new Model(name, path + "horse/horse_statue_01_1k.obj");
			break;
		case shelf_model:
			// ÎïÆ·¼Ü
			model = new Model(name, path + "wooden_display_shelves/wooden_display_shelves_01_1k.obj");
			break;
		case cat_statue:
			// Ã¨µñÏñ
			model = new Model(name, path + "cat_statue/concrete_cat_statue_1k.obj");
			break;
		case marble_bust:
			// ÈËÍ·µñÏñ
			model = new Model(name, path + "marble_bust/marble_bust_01_1k.obj");
			break;
		case wooden_elephant:
			// ´óÏóÄ¾µñ
			model = new Model(name, path + "wooden_elephant/carved_wooden_elephant_1k.obj");
			break;
		case ceramic_vase_01:
			// ÌÕ´ÉÆ¿A
			model = new Model(name, path + "ceramic_vase_01/ceramic_vase_02_1k.obj");
			break;
		case ceramic_vase_02:
			// ÌÕ´ÉÆ¿B
			model = new Model(name, path + "ceramic_vase_02/ceramic_vase_03_1k.obj");
			break;
		case ceramic_vase_03:
			// ÌÕ´ÉÆ¿C
			model = new Model(name, path + "ceramic_vase_03/ceramic_vase_04_1k.obj");
			break;
		case duck:
			// Ð¡»ÆÑ¼
			model = new Model(name, path + "duck/rubber_duck_toy_1k.obj");
			break;
		case garden_gnome:
			// »¨Ô°Ð¡¾«Áé
			model = new Model(name, path + "garden_gnome/garden_gnome_1k.obj");
			break;
		case lubricant_spray:
			// ÅçÆá¹Þ
			model = new Model(name, path + "lubricant_spray/lubricant_spray_1k.obj");
			break;
		case cabinet:
			// Ä¾³÷¹ñ
			model = new Model(name, path + "cabinet/modern_wooden_cabinet_1k.obj");
			break;
		case white_table:
			// °××À
			model = new Model(name, path + "white_table/coffee_table_round_01_1k.obj");
			break;
		case white_chair:
			// °×ÒÎ
			model = new Model(name, path + "white_chair/ArmChair_01_1k.obj");
			break;
		case black_chair:
			// ºÚÒÎ
			model = new Model(name, path + "black_chair/modern_arm_chair_01_1k.obj");
			break;
		case basket:
			// Ä¾Àº
			model = new Model(name, path + "wicker_basket/wicker_basket_01_1k.obj");
			break;
		case pear:
			// Àæ×Ó
			model = new Model(name, path + "pears/food_pears_asian_01_1k.obj");
			break;
		case apple:
			// Æ»¹û
			model = new Model(name, path + "apple/food_apple_01_1k.obj");
			break;
		case kiwi:
			// ÆæÒì¹û
			model = new Model(name, path + "kiwi/food_kiwi_01_1k.obj");
			break;
		case avocado:
			// öùÀæ
			model = new Model(name, path + "avocado/food_avocado_01_1k.obj");
			break;
		case dartboard:
			// ±ê°Ð
			model = new Model(name, path + "dartboard/dartboard_1k.obj");
			break;
		case chess_set:
			// ÆåÅÌ
			model = new Model(name, path + "chess_set/chess_set_1k.obj");
			break;
		case tea_set:
			// ²è¾ß
			model = new Model(name, path + "tea_set/tea_set_01_1k.obj");
			break;
		default:
			break;
	}

	return model;
}

// ³¡¾°Ô¤¼ÓÔØ

void LoadPresets::preLoadModel(std::vector<Object*>& sceneObjects) {
	Object* objectModel;

	// ±³¾°°å
	objectModel = LoadPresets::loadModel(background_model, u8"±³¾°°å");
	objectModel->position = glm::vec3(-0.4f, -2.5f, -13.0f);
	objectModel->rotation = glm::vec3(0.0f, -30.0f, 0.0f);
	objectModel->scale = glm::vec3(0.7f, 0.7f, 0.7f);
	sceneObjects.push_back(objectModel);

	// ÎïÆ·¼Ü
	objectModel = LoadPresets::loadModel(shelf_model, u8"ÎïÆ·¼Ü");
	objectModel->position = glm::vec3(3.721f, -2.220f, -10.627f);
	objectModel->rotation = glm::vec3(0.0f, -30.0f, 0.0f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// ÈËÍ·µñÏñ
	objectModel = LoadPresets::loadModel(marble_bust, u8"ÈËÍ·µñÏñ");
	objectModel->position = glm::vec3(2.742f, 1.273f, -11.071f);   
	objectModel->rotation = glm::vec3(0.0f, 13.00f, 0.0f);
	objectModel->scale = glm::vec3(2.0f, 2.0f, 2.0f);
	sceneObjects.push_back(objectModel);

	// Ã¨µñÏñ
	objectModel = LoadPresets::loadModel(cat_statue, u8"Ã¨µñÏñ");
	objectModel->position = glm::vec3(4.631f, 1.269f, -10.049f);
	objectModel->rotation = glm::vec3(0.0f, -4.40f, 0.0f);
	objectModel->scale = glm::vec3(3.5f, 3.5f, 3.5f);
	sceneObjects.push_back(objectModel);

	// ´óÏóÄ¾µñ
	objectModel = LoadPresets::loadModel(wooden_elephant, u8"´óÏóµñÏñ");
	objectModel->position = glm::vec3(3.725f, 1.275f, -10.621f);
	objectModel->rotation = glm::vec3(0.0f, -7.30f, 0.0f);
	objectModel->scale = glm::vec3(10.0f, 10.0f, 10.0f);
	sceneObjects.push_back(objectModel);

	// ÌÕ´ÉÆ¿A
	objectModel = LoadPresets::loadModel(ceramic_vase_01, u8"ÌÕ´ÉÆ¿A");
	objectModel->position = glm::vec3(2.835f, 0.144f, -11.179f);
	objectModel->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// ÌÕ´ÉÆ¿B
	objectModel = LoadPresets::loadModel(ceramic_vase_02, u8"ÌÕ´ÉÆ¿B");
	objectModel->position = glm::vec3(3.690f, 0.148f, -10.582f);
	objectModel->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objectModel->scale = glm::vec3(2.3f, 2.3f, 2.3f);
	sceneObjects.push_back(objectModel);

	// ÌÕ´ÉÆ¿C
	objectModel = LoadPresets::loadModel(ceramic_vase_03, u8"ÌÕ´ÉÆ¿C");
	objectModel->position = glm::vec3(4.628f, 0.146f, -10.083f);
	objectModel->rotation = glm::vec3(0.0f, -38.30f, 0.0f);
	objectModel->scale = glm::vec3(2.8f, 2.8f, 2.8f);
	sceneObjects.push_back(objectModel);

	// ÅçÆá¹Þ
	objectModel = LoadPresets::loadModel(lubricant_spray, u8"ÅçÆá¹Þ");
	objectModel->position = glm::vec3(2.792f, -0.980f, -11.103f);
	objectModel->rotation = glm::vec3(0.0f, -41.70f, 0.0f);
	objectModel->scale = glm::vec3(4.0f, 4.0f, 4.0f);
	sceneObjects.push_back(objectModel);

	// ÅçÆá¹Þ
	objectModel = LoadPresets::loadModel(lubricant_spray, u8"ÅçÆá¹Þ");
	objectModel->position = glm::vec3(2.792f, -0.980f, -11.103f);
	objectModel->rotation = glm::vec3(0.0f, -41.70f, 0.0f);
	objectModel->scale = glm::vec3(4.0f, 4.0f, 4.0f);
	sceneObjects.push_back(objectModel);

	// Ð¡»ÆÑ¼
	objectModel = LoadPresets::loadModel(duck, u8"Ð¡»ÆÑ¼");
	objectModel->position = glm::vec3(3.677f, -0.991f, -10.571f);
	objectModel->rotation = glm::vec3(0.0f, 11.30f, 0.0f);
	objectModel->scale = glm::vec3(2.0f, 2.0f, 2.0f);
	sceneObjects.push_back(objectModel);

	// »¨Ô°Ð¡¾«Áé
	objectModel = LoadPresets::loadModel(garden_gnome, u8"»¨Ô°Ð¡¾«Áé");
	objectModel->position = glm::vec3(4.490f, -0.980f, -9.938f);
	objectModel->rotation = glm::vec3(0.0f, -30.00f, 0.0f);
	objectModel->scale = glm::vec3(1.2f, 1.2f, 1.2f);
	sceneObjects.push_back(objectModel);

	// °××À
	objectModel = LoadPresets::loadModel(white_table, u8"°××À");
	objectModel->position = glm::vec3(0.200f, -2.212f, -8.018f);
	objectModel->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objectModel->scale = glm::vec3(2.0f, 2.0f, 2.0f);
	sceneObjects.push_back(objectModel);

	// °×ÒÎ
	objectModel = LoadPresets::loadModel(white_chair, u8"°×ÒÎ");
	objectModel->position = glm::vec3(2.411f, -2.210f, -7.979f);
	objectModel->rotation = glm::vec3(0.0f, -86.40f, 0.0f);
	objectModel->scale = glm::vec3(2.0f, 2.0f, 2.0f);
	sceneObjects.push_back(objectModel);

	// ºÚÒÎ
	objectModel = LoadPresets::loadModel(black_chair, u8"ºÚÒÎ");
	objectModel->position = glm::vec3(-1.676f, -2.219f, -7.404f);
	objectModel->rotation = glm::vec3(0.0f, 106.70f, 0.0f);
	objectModel->scale = glm::vec3(1.8f, 1.8f, 1.8f);
	sceneObjects.push_back(objectModel);

	// Ä¾³÷¹ñ
	objectModel = LoadPresets::loadModel(cabinet, u8"Ä¾³÷¹ñ");
	objectModel->position = glm::vec3(-1.414f, -2.215f, -11.117f);
	objectModel->rotation = glm::vec3(0.0f, 59.90f, 0.0f);
	objectModel->scale = glm::vec3(2.0f, 2.0f, 2.0f);
	sceneObjects.push_back(objectModel);

	// Ä¾Àº
	objectModel = LoadPresets::loadModel(basket, u8"Ä¾Àº");
	objectModel->position = glm::vec3(-0.792f, -0.856f, -12.183f);
	objectModel->rotation = glm::vec3(0.0f, 58.50f, 0.0f);			 
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// Àæ×Ó
	objectModel = LoadPresets::loadModel(pear, u8"Àæ×Ó");
	objectModel->position = glm::vec3(-0.804f, -0.845f, -12.206f);
	objectModel->rotation = glm::vec3(0.0f, 96.30f, 0.0f);
	objectModel->scale = glm::vec3(6.0f, 6.0f, 6.0f);
	sceneObjects.push_back(objectModel);

	// Æ»¹û
	objectModel = LoadPresets::loadModel(apple, u8"Æ»¹û");				   
	objectModel->position = glm::vec3(-0.782f, -0.729f, -11.734f);
	objectModel->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// Æ»¹û
	objectModel = LoadPresets::loadModel(apple, u8"Æ»¹û");
	objectModel->position = glm::vec3(-0.759f, -0.775f, -12.688f);
	objectModel->rotation = glm::vec3(45.40f, 52.60f, 32.40f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// ÆæÒì¹û
	objectModel = LoadPresets::loadModel(kiwi, u8"ÆæÒì¹û");
	objectModel->position = glm::vec3(-1.115f, -0.565f, -12.279f);
	objectModel->rotation = glm::vec3(6.60f, 115.50f, -80.40f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// öùÀæ
	objectModel = LoadPresets::loadModel(avocado, u8"öùÀæ");
	objectModel->position = glm::vec3(-0.869f, -0.516f, -12.192f);
	objectModel->rotation = glm::vec3(-77.80f, -10.40f, -139.60f);
	objectModel->scale = glm::vec3(2.3f, 2.3f, 2.3f);
	sceneObjects.push_back(objectModel);

	// ±ê°Ð
	objectModel = LoadPresets::loadModel(dartboard, u8"±ê°Ð");
	objectModel->position = glm::vec3(-1.690f, 1.383f, -11.745f);
	objectModel->rotation = glm::vec3(0.0f, 59.10f, 0.0f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// ÆåÅÌ
	objectModel = LoadPresets::loadModel(chess_set, u8"ÆåÅÌ");
	objectModel->position = glm::vec3(0.220f, -1.236f, -8.015f);
	objectModel->rotation = glm::vec3(0.0f, 11.90f, 0.0f);
	objectModel->scale = glm::vec3(2.8f, 2.8f, 2.8f);
	sceneObjects.push_back(objectModel);

	// ÅèÔÔ
	objectModel = LoadPresets::loadModel(plant_model, u8"ÅèÔÔ");
	objectModel->position = glm::vec3(-1.317f, -0.867f, -11.174f);
	objectModel->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// ²è¾ß
	objectModel = LoadPresets::loadModel(tea_set, u8"²è¾ß");
	objectModel->position = glm::vec3(-1.954f, -0.855f, -9.923f);
	objectModel->rotation = glm::vec3(0.0f, 61.10f, 0.0f);
	objectModel->scale = glm::vec3(2.0f, 2.0f, 2.0f);
	sceneObjects.push_back(objectModel);

}

