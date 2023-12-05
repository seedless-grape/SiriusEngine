/* 载入预设物体 */

#ifndef LOAD_PRESETS_H_
#define LOAD_PRESETS_H_

#include "cube.h"
#include "model.h"
#include "skybox.h"

enum objectType {
    White_Box, Wooden_Box
};

enum modelType {
    plant_model, background_model, horse_model, shelf_model, cat_statue, marble_bust, wooden_elephant,
    ceramic_vase_01, ceramic_vase_02, ceramic_vase_03, duck, garden_gnome, lubricant_spray, cabinet,
    white_table, white_chair, black_chair, basket, pear, apple, kiwi, avocado, dartboard, chess_set, tea_set
};


// 加载预设物体
class LoadPresets {
public:
    // 预加载材质、着色器
    static void preLoadShaderTexture();

    // 加载点光源
    static PointLight* loadPointLight();

    // 加载天空盒
    static Object* loadSkybox();

    // 加载立方体
    static Object* loadCube(objectType type, std::string name);

    // 加载模型
    static Object* loadModel(modelType type, std::string name);

public:
    // 场景预加载

    // 预加载场景模型
    static void preLoadModel(std::vector<Object*>& sceneObjects);

    // 预加载场景光照
    //static void preLoadLight(std::vector<Light*>& sceneLights);
};

#endif