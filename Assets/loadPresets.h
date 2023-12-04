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
    duck_model, background_model
};

// 加载预设物体
class LoadPresets {
public:

    // 预加载
    static void preLoad();

    // 加载点光源
    static PointLight* loadPointLight();

    // 加载天空盒
    static Object* loadSkybox();

    // 加载立方体

    static Object* loadCube(objectType type, std::string name);


    // 加载模型
    static Object* loadModel(modelType type, std::string name);

};

#endif