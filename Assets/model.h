#ifndef MODEL_H
#define MODEL_H

/* ģ�� */

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "Core/shader.h"
#include "modelRenderer.h"
#include "Core/object.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// ���ļ��ж�ȡ������ͼ
//unsigned int textureFromFile(const char* path, const std::string& directory);

// ģ����
class Model : public Object {
public:
    std::vector<Mesh> meshes; // ģ������
    std::string directory; // ģ���ļ�·��

    // ���캯��
    Model(std::string name, std::string const& path, bool gamma = false);

    // ����ģ��
    void draw(Shader& shader);

    void draw(Renderer& renderer, bool drawCoordinate = true) override;

private:
    // ����ģ��
    void loadModel(std::string const& path);

    // ������
    void processNode(aiNode* node, const aiScene* scene);

    // ����������
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    // ���ز�����ͼ
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
                                              aiTextureType type,
                                              std::string typeName);
};

#endif