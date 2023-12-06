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

enum modelType {
    plant_model, background_model, horse_model, shelf_model, cat_statue, marble_bust, wooden_elephant,
    ceramic_vase_01, ceramic_vase_02, ceramic_vase_03, duck, garden_gnome, lubricant_spray, cabinet,
    white_table, white_chair, black_chair, basket, pear, apple, kiwi, avocado, dartboard, chess_set, tea_set
};


// ����Ԥ������
class LoadPresets {
public:
    // Ԥ���ز��ʡ���ɫ��
    static void preLoadShaderTexture();

    // ���ص��Դ
    static PointLight* loadPointLight();

    // ������պ�
    static Object* loadSkybox();

    // ����������
    static Object* loadCube(objectType type, std::string name);

    // ����ģ��
    static Object* loadModel(modelType type, std::string name);

public:
    // ����Ԥ����

    // Ԥ���س���ģ��
    static void preLoadModel(std::vector<Object*>& sceneObjects);
};

#endif