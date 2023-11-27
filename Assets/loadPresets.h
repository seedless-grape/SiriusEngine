/* ����Ԥ������ */

#ifndef LOAD_PRESETS_H_
#define LOAD_PRESETS_H_

#include "cube.h"
#include "model.h"

enum objectType {
    White_Box, Wooden_Box
};

enum modelType {
    duck_model
};

// ����Ԥ������
class LoadPresets {
public:
    // ���ص��Դ
    static PointLight* loadPointLight();

    // ����������
    static Object* loadCube(objectType type, std::string name);

    // ����ģ��
    static Object* loadModel(modelType type, std::string name);
};

#endif