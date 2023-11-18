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

class Shader;

// 从文件中读取材质贴图
unsigned int textureFromFile(const char* path, const std::string& directory,
                             bool gamma = false);

// 模型类
class Model : public Object {
public:
	std::vector<Texture> texturesLoaded; // 记录已加载的模型
	std::vector<Mesh> meshes; // 模型网格
	std::string directory; // 模型文件路径
	bool gammaCorrection; // ？

    // 构造/析构函数
    Model(std::string const& path, std::string _name = "Model", bool gamma = false);

	Model(glm::vec3 _position, std::string const& path,
		  std::string _name = "Model", bool gamma = false,
		  glm::vec3 _rotaion = glm::vec3(0.0f),
		  glm::vec3 _scale = glm::vec3(1.0f),
		  glm::vec3 _color = glm::vec3(1.0f));

    ~Model() override = default;

    // 绘制模型
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

// 模型渲染类
class ModelRenderer : public Renderer {
public:
    // 构造/析构函数
	ModelRenderer(const Shader& shader);

	~ModelRenderer();

	// 渲染模型
	void render(const Object& object, bool drawCoordinate = true) override;

private:
	unsigned int modelVAO;

private:
	void initRenderData() override;

};

#endif