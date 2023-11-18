/* ����Ԥ������ */

#ifndef LOAD_PRESETS_H_
#define LOAD_PRESETS_H_

#include "cube.h"

enum objectType {
    White_Box, Wooden_Box
};

// ����Ԥ������
class LoadPresets {
public:
    // ���ص��Դ
    static PointLight* loadPointLight();

    // ����������
    static Object* loadCube(objectType type, std::string name);

};

#endif