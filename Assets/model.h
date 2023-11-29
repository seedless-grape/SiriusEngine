#ifndef MODEL_H
#define MODEL_H

/* 模型 */

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

// 从文件中读取材质贴图
//unsigned int textureFromFile(const char* path, const std::string& directory);

// 模型类
class Model : public Object {
public:
    std::vector<Mesh> meshes; // 模型网格
    std::string directory; // 模型文件路径

    // 构造函数
    Model(std::string name, std::string const& path, bool gamma = false);

    // 绘制模型
    void draw(Shader& shader);

    void draw(Renderer& renderer, bool drawCoordinate = true) override;

private:
    // 加载模型
    void loadModel(std::string const& path);

    // 处理结点
    void processNode(aiNode* node, const aiScene* scene);

    // 处理结点网格
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    // 加载材质贴图
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
                                              aiTextureType type,
                                              std::string typeName);
};

#endif