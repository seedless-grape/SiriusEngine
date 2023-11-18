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
#include "Core/object.h"
#include "Core/renderer.h"

class Shader;

// ���ļ��ж�ȡ������ͼ
unsigned int textureFromFile(const char* path, const std::string& directory,
                             bool gamma = false);

// ģ����
class Model : public Object {
public:
	std::vector<Texture> texturesLoaded; // ��¼�Ѽ��ص�ģ��
	std::vector<Mesh> meshes; // ģ������
	std::string directory; // ģ���ļ�·��
	bool gammaCorrection; // ��

    // ����/��������
    Model(std::string const& path, std::string _name = "Model", bool gamma = false);

	Model(glm::vec3 _position, std::string const& path,
		  std::string _name = "Model", bool gamma = false,
		  glm::vec3 _rotaion = glm::vec3(0.0f),
		  glm::vec3 _scale = glm::vec3(1.0f),
		  glm::vec3 _color = glm::vec3(1.0f));

    ~Model() override = default;

    // ����ģ��
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

// ģ����Ⱦ��
class ModelRenderer : public Renderer {
public:
    // ����/��������
	ModelRenderer(const Shader& shader);

	~ModelRenderer();

	// ��Ⱦģ��
	void render(const Object& object, bool drawCoordinate = true) override;

private:
	unsigned int modelVAO;

private:
	void initRenderData() override;

};

#endif