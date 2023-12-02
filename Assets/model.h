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
#include "Core/object.h"
#include "Core/renderer.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// 模型类
class Model : public Object {
public:
    std::vector<Mesh> meshes; // 模型网格
    std::string directory; // 模型文件路径

    // 构造函数
    Model(std::string name, std::string const& path, bool gamma = false);

    // 绘制模型
    void draw(Renderer& renderer, Shadow* shadow = nullptr, bool drawCoordinate = true, bool gamma = false) override;

    // 阴影绘制
    void shadowDraw(Renderer& renderer) override;

    // 阴影模型绘制
    void shadowModelDraw(Renderer& renderer, Shadow* shadow, bool drawCoordinate = true, bool gamma = false) override;

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

// 模型渲染类
class ModelRenderer : public Renderer {
public:
    // 构造/析构函数
    ModelRenderer(const Shader& shader);

    ~ModelRenderer() override = default;

    // 渲染
    void render(const Object& object, bool drawCoordinate = true, bool gamma = false) override;
};

#endif