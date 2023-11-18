/* 载入预设物体 */

#ifndef LOAD_PRESETS_H_
#define LOAD_PRESETS_H_

#include "cube.h"

enum objectType {
    White_Box, Wooden_Box
};

// 加载预设物体
class LoadPresets {
public:
    // 加载点光源
    static PointLight* loadPointLight();

    // 加载立方体
    static Object* loadCube(objectType type, std::string name);

};

#endif