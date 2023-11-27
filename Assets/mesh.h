#ifndef MESH_H_
#define MESH_H_

/* ����ģ�� */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Core/shader.h"

#include <vector>
#include <string>

// Ӱ�춥�����������
#define MAX_BONE_INFLUENCE 4

// �������Խṹ��
struct Vertex {
	glm::vec3 position; // λ������
	glm::vec3 normal; // ���߷���
	glm::vec2 texCoords; // ��������
	glm::vec3 tangent; // ���߷���
	glm::vec3 bitangent; // �����߷���

	int mBoneIDs[MAX_BONE_INFLUENCE]; // Ӱ��ö���Ĺ�������
	float mWeights[MAX_BONE_INFLUENCE]; // Ӱ��ö���Ĺ���Ӱ��Ȩ��
};

// �������Խṹ��
struct TextureS {
	unsigned int id; // ����id
	std::string type; // ��������
	std::string path; // ����·��
};

// ������
class Mesh {
public:
	// ��������
	std::vector<Vertex> vertices; // ����
	std::vector<unsigned int> indices; // ͼ��(������)����
	std::vector<TextureS> textures; // ����

public:
	// ���캯��
	Mesh(std::vector<Vertex> _vertices,
		 std::vector<unsigned int> _indices,
		 std::vector<TextureS> _textures);

	// �������
	void draw(Shader& shader);

private:
	// ��Ⱦ����
	unsigned int VAO, VBO, EBO;

	// ��Ⱦ����ͼ��
	void setupMesh();
};

#endif // !MESH_H_
