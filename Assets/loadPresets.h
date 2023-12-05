/* ����Ԥ������ */

#ifndef LOAD_PRESETS_H_
#define LOAD_PRESETS_H_

#include "cube.h"
#include "model.h"
#include "skybox.h"

enum objectType {
    White_Box, Wooden_Box
};

enum modelType {
    plant_model, background_model, horse_model
};

// ����Ԥ������
class LoadPresets {
public:
    // Ԥ���ز��ʡ���ɫ��
    static void preLoad();

    // ���ص��Դ
    static PointLight* loadPointLight();

    // ������պ�
    static Object* loadSkybox();

    // ����������
    static Object* loadCube(objectType type, std::string name);

    // ����ģ��
    static Object* loadModel(modelType type, std::string name);
};

#endif