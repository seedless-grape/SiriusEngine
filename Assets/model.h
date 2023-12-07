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
    std::vector<Mesh> meshes; // ģ������
    std::string directory; // ģ���ļ�·��

    // ���캯��
    Model(std::string name, std::string const& path, bool gamma = false);

    // ����ģ��
    void draw(Renderer& renderer, Shadow* shadow = nullptr, bool drawCoordinate = true, bool gamma = false) override;

    // ��Ӱ����
    void shadowDraw(Renderer& renderer) override;

    // ��Ӱģ�ͻ���
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

// ģ����Ⱦ��
class ModelRenderer : public Renderer {
public:
    ModelRenderer(const Shader& shader);

    ~ModelRenderer() override = default;

    void render(const Object& object, bool drawCoordinate = true, bool gamma = false) override;
};

#endif