#include "loadPresets.h"
#include "resource_manager.h"
#include "Core/light.h"


void LoadPresets::preLoadShaderTexture() {

	// 加载着色器
	ResourceManager::loadShader("Shader/light_cube.vert", "Shader/light_cube.frag", nullptr, "light_cube");
	ResourceManager::loadShader("Shader/object.vert", "Shader/object.frag", nullptr, "object");
	ResourceManager::loadShader("Shader/model.vert", "Shader/model.frag", nullptr, "model");
	ResourceManager::loadShader("Shader/pbr.vert", "Shader/pbr.frag", nullptr, "pbr");
	ResourceManager::loadShader("Shader/coordinate.vert", "Shader/coordinate.frag", nullptr, "coordinate");
	ResourceManager::loadShader("Shader/skybox.vert", "Shader/skybox.frag", nullptr, "skybox");
	ResourceManager::loadShader("Shader/msaa.vert", "Shader/msaa.frag", nullptr, "msaa");
	ResourceManager::loadShader("Shader/shadow.vert", "Shader/shadow.frag", nullptr, "shadow");
	ResourceManager::loadShader("Shader/hdr.vert", "Shader/hdr.frag", nullptr, "hdr");

	// 加载材质
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

	// 载入白砖方块
	if (type == White_Box) {
		cube = new Cube(name);
		cube->position = glm::vec3(0.0f, 0.0f, -3.0f);

		cube->diffuseTexture = ResourceManager::getTexture("white_tiles");
		cube->specularTexture = ResourceManager::getTexture("pure_black");
		cube->shininess = 64.0f;
	}

	// 载入木箱
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
			// 盆栽
			model = new Model(name, path + "potted_plant/potted_plant_04_2k.obj");
			break;
		case background_model:
			// 背景板
			model = new Model(name, path + "background/background.obj");
			break;
		case horse_model:
			// 马雕塑
			model = new Model(name, path + "horse/horse_statue_01_1k.obj");
			break;
		case shelf_model:
			// 物品架
			model = new Model(name, path + "wooden_display_shelves/wooden_display_shelves_01_1k.obj");
			break;
		case cat_statue:
			// 猫雕像
			model = new Model(name, path + "cat_statue/concrete_cat_statue_1k.obj");
			break;
		case marble_bust:
			// 人头雕像
			model = new Model(name, path + "marble_bust/marble_bust_01_1k.obj");
			break;
		case wooden_elephant:
			// 大象木雕
			model = new Model(name, path + "wooden_elephant/carved_wooden_elephant_1k.obj");
			break;
		case ceramic_vase_01:
			// 陶瓷瓶A
			model = new Model(name, path + "ceramic_vase_01/ceramic_vase_02_1k.obj");
			break;
		case ceramic_vase_02:
			// 陶瓷瓶B
			model = new Model(name, path + "ceramic_vase_02/ceramic_vase_03_1k.obj");
			break;
		case ceramic_vase_03:
			// 陶瓷瓶C
			model = new Model(name, path + "ceramic_vase_03/ceramic_vase_04_1k.obj");
			break;
		case duck:
			// 小黄鸭
			model = new Model(name, path + "duck/rubber_duck_toy_1k.obj");
			break;
		case garden_gnome:
			// 花园小精灵
			model = new Model(name, path + "garden_gnome/garden_gnome_1k.obj");
			break;
		case lubricant_spray:
			// 喷漆罐
			model = new Model(name, path + "lubricant_spray/lubricant_spray_1k.obj");
			break;
		case cabinet:
			// 木橱柜
			model = new Model(name, path + "cabinet/modern_wooden_cabinet_1k.obj");
			break;
		case white_table:
			// 白桌
			model = new Model(name, path + "white_table/coffee_table_round_01_1k.obj");
			break;
		case white_chair:
			// 白椅
			model = new Model(name, path + "white_chair/ArmChair_01_1k.obj");
			break;
		case black_chair:
			// 黑椅
			model = new Model(name, path + "black_chair/modern_arm_chair_01_1k.obj");
			break;
		case basket:
			// 木篮
			model = new Model(name, path + "wicker_basket/wicker_basket_01_1k.obj");
			break;
		case pear:
			// 梨子
			model = new Model(name, path + "pears/food_pears_asian_01_1k.obj");
			break;
		case apple:
			// 苹果
			model = new Model(name, path + "apple/food_apple_01_1k.obj");
			break;
		case kiwi:
			// 奇异果
			model = new Model(name, path + "kiwi/food_kiwi_01_1k.obj");
			break;
		case avocado:
			// 鳄梨
			model = new Model(name, path + "avocado/food_avocado_01_1k.obj");
			break;
		case dartboard:
			// 标靶
			model = new Model(name, path + "dartboard/dartboard_1k.obj");
			break;
		case chess_set:
			// 棋盘
			model = new Model(name, path + "chess_set/chess_set_1k.obj");
			break;
		case tea_set:
			// 茶具
			model = new Model(name, path + "tea_set/tea_set_01_1k.obj");
			break;
		default:
			break;
	}

	return model;
}

// 场景预加载

void LoadPresets::preLoadModel(std::vector<Object*>& sceneObjects) {
	Object* objectModel;

	// 背景板
	objectModel = LoadPresets::loadModel(background_model, u8"背景板");
	objectModel->position = glm::vec3(-0.4f, -2.5f, -13.0f);
	objectModel->rotation = glm::vec3(0.0f, -30.0f, 0.0f);
	objectModel->scale = glm::vec3(0.7f, 0.7f, 0.7f);
	sceneObjects.push_back(objectModel);

	// 物品架
	objectModel = LoadPresets::loadModel(shelf_model, u8"物品架");
	objectModel->position = glm::vec3(3.721f, -2.220f, -10.627f);
	objectModel->rotation = glm::vec3(0.0f, -30.0f, 0.0f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// 人头雕像
	objectModel = LoadPresets::loadModel(marble_bust, u8"人头雕像");
	objectModel->position = glm::vec3(2.742f, 1.273f, -11.071f);   
	objectModel->rotation = glm::vec3(0.0f, 13.00f, 0.0f);
	objectModel->scale = glm::vec3(2.0f, 2.0f, 2.0f);
	sceneObjects.push_back(objectModel);

	// 猫雕像
	objectModel = LoadPresets::loadModel(cat_statue, u8"猫雕像");
	objectModel->position = glm::vec3(4.631f, 1.269f, -10.049f);
	objectModel->rotation = glm::vec3(0.0f, -4.40f, 0.0f);
	objectModel->scale = glm::vec3(3.5f, 3.5f, 3.5f);
	sceneObjects.push_back(objectModel);

	// 大象木雕
	objectModel = LoadPresets::loadModel(wooden_elephant, u8"大象雕像");
	objectModel->position = glm::vec3(3.725f, 1.275f, -10.621f);
	objectModel->rotation = glm::vec3(0.0f, -7.30f, 0.0f);
	objectModel->scale = glm::vec3(10.0f, 10.0f, 10.0f);
	sceneObjects.push_back(objectModel);

	// 陶瓷瓶A
	objectModel = LoadPresets::loadModel(ceramic_vase_01, u8"陶瓷瓶A");
	objectModel->position = glm::vec3(2.835f, 0.144f, -11.179f);
	objectModel->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// 陶瓷瓶B
	objectModel = LoadPresets::loadModel(ceramic_vase_02, u8"陶瓷瓶B");
	objectModel->position = glm::vec3(3.690f, 0.148f, -10.582f);
	objectModel->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objectModel->scale = glm::vec3(2.3f, 2.3f, 2.3f);
	sceneObjects.push_back(objectModel);

	// 陶瓷瓶C
	objectModel = LoadPresets::loadModel(ceramic_vase_03, u8"陶瓷瓶C");
	objectModel->position = glm::vec3(4.628f, 0.146f, -10.083f);
	objectModel->rotation = glm::vec3(0.0f, -38.30f, 0.0f);
	objectModel->scale = glm::vec3(2.8f, 2.8f, 2.8f);
	sceneObjects.push_back(objectModel);

	// 喷漆罐
	objectModel = LoadPresets::loadModel(lubricant_spray, u8"喷漆罐");
	objectModel->position = glm::vec3(2.792f, -0.980f, -11.103f);
	objectModel->rotation = glm::vec3(0.0f, -41.70f, 0.0f);
	objectModel->scale = glm::vec3(4.0f, 4.0f, 4.0f);
	sceneObjects.push_back(objectModel);

	// 喷漆罐
	objectModel = LoadPresets::loadModel(lubricant_spray, u8"喷漆罐");
	objectModel->position = glm::vec3(2.792f, -0.980f, -11.103f);
	objectModel->rotation = glm::vec3(0.0f, -41.70f, 0.0f);
	objectModel->scale = glm::vec3(4.0f, 4.0f, 4.0f);
	sceneObjects.push_back(objectModel);

	// 小黄鸭
	objectModel = LoadPresets::loadModel(duck, u8"小黄鸭");
	objectModel->position = glm::vec3(3.677f, -0.991f, -10.571f);
	objectModel->rotation = glm::vec3(0.0f, 11.30f, 0.0f);
	objectModel->scale = glm::vec3(2.0f, 2.0f, 2.0f);
	sceneObjects.push_back(objectModel);

	// 花园小精灵
	objectModel = LoadPresets::loadModel(garden_gnome, u8"花园小精灵");
	objectModel->position = glm::vec3(4.490f, -0.980f, -9.938f);
	objectModel->rotation = glm::vec3(0.0f, -30.00f, 0.0f);
	objectModel->scale = glm::vec3(1.2f, 1.2f, 1.2f);
	sceneObjects.push_back(objectModel);

	// 白桌
	objectModel = LoadPresets::loadModel(white_table, u8"白桌");
	objectModel->position = glm::vec3(0.200f, -2.212f, -8.018f);
	objectModel->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objectModel->scale = glm::vec3(2.0f, 2.0f, 2.0f);
	sceneObjects.push_back(objectModel);

	// 白椅
	objectModel = LoadPresets::loadModel(white_chair, u8"白椅");
	objectModel->position = glm::vec3(2.411f, -2.210f, -7.979f);
	objectModel->rotation = glm::vec3(0.0f, -86.40f, 0.0f);
	objectModel->scale = glm::vec3(2.0f, 2.0f, 2.0f);
	sceneObjects.push_back(objectModel);

	// 黑椅
	objectModel = LoadPresets::loadModel(black_chair, u8"黑椅");
	objectModel->position = glm::vec3(-1.676f, -2.219f, -7.404f);
	objectModel->rotation = glm::vec3(0.0f, 106.70f, 0.0f);
	objectModel->scale = glm::vec3(1.8f, 1.8f, 1.8f);
	sceneObjects.push_back(objectModel);

	// 木橱柜
	objectModel = LoadPresets::loadModel(cabinet, u8"木橱柜");
	objectModel->position = glm::vec3(-1.414f, -2.215f, -11.117f);
	objectModel->rotation = glm::vec3(0.0f, 59.90f, 0.0f);
	objectModel->scale = glm::vec3(2.0f, 2.0f, 2.0f);
	sceneObjects.push_back(objectModel);

	// 木篮
	objectModel = LoadPresets::loadModel(basket, u8"木篮");
	objectModel->position = glm::vec3(-0.792f, -0.856f, -12.183f);
	objectModel->rotation = glm::vec3(0.0f, 58.50f, 0.0f);			 
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// 梨子
	objectModel = LoadPresets::loadModel(pear, u8"梨子");
	objectModel->position = glm::vec3(-0.804f, -0.845f, -12.206f);
	objectModel->rotation = glm::vec3(0.0f, 96.30f, 0.0f);
	objectModel->scale = glm::vec3(6.0f, 6.0f, 6.0f);
	sceneObjects.push_back(objectModel);

	// 苹果
	objectModel = LoadPresets::loadModel(apple, u8"苹果");				   
	objectModel->position = glm::vec3(-0.782f, -0.729f, -11.734f);
	objectModel->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// 苹果
	objectModel = LoadPresets::loadModel(apple, u8"苹果");
	objectModel->position = glm::vec3(-0.759f, -0.775f, -12.688f);
	objectModel->rotation = glm::vec3(45.40f, 52.60f, 32.40f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// 奇异果
	objectModel = LoadPresets::loadModel(kiwi, u8"奇异果");
	objectModel->position = glm::vec3(-1.115f, -0.565f, -12.279f);
	objectModel->rotation = glm::vec3(6.60f, 115.50f, -80.40f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// 鳄梨
	objectModel = LoadPresets::loadModel(avocado, u8"鳄梨");
	objectModel->position = glm::vec3(-0.869f, -0.516f, -12.192f);
	objectModel->rotation = glm::vec3(-77.80f, -10.40f, -139.60f);
	objectModel->scale = glm::vec3(2.3f, 2.3f, 2.3f);
	sceneObjects.push_back(objectModel);

	// 标靶
	objectModel = LoadPresets::loadModel(dartboard, u8"标靶");
	objectModel->position = glm::vec3(-1.690f, 1.383f, -11.745f);
	objectModel->rotation = glm::vec3(0.0f, 59.10f, 0.0f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// 棋盘
	objectModel = LoadPresets::loadModel(chess_set, u8"棋盘");
	objectModel->position = glm::vec3(0.220f, -1.236f, -8.015f);
	objectModel->rotation = glm::vec3(0.0f, 11.90f, 0.0f);
	objectModel->scale = glm::vec3(2.8f, 2.8f, 2.8f);
	sceneObjects.push_back(objectModel);

	// 盆栽
	objectModel = LoadPresets::loadModel(plant_model, u8"盆栽");
	objectModel->position = glm::vec3(-1.317f, -0.867f, -11.174f);
	objectModel->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objectModel->scale = glm::vec3(3.0f, 3.0f, 3.0f);
	sceneObjects.push_back(objectModel);

	// 茶具
	objectModel = LoadPresets::loadModel(tea_set, u8"茶具");
	objectModel->position = glm::vec3(-1.954f, -0.855f, -9.923f);
	objectModel->rotation = glm::vec3(0.0f, 61.10f, 0.0f);
	objectModel->scale = glm::vec3(2.0f, 2.0f, 2.0f);
	sceneObjects.push_back(objectModel);
}

