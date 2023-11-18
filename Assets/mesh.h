/* 网格模型 */

#ifndef MESH_H_
#define MESH_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>

// 影响顶点的最大骨骼数
#define MAX_BONE_INFLUENCE 4

class Shader;

// 顶点属性结构体
struct Vertex {
	glm::vec3 position; // 位置坐标
	glm::vec3 normal; // 法线方向
	glm::vec2 texCoords; // 纹理坐标
	glm::vec3 tangent; // 切线方向
	glm::vec3 bitangent; // 副切线方向

	int mBoneIDs[MAX_BONE_INFLUENCE]; // 影响该顶点的骨骼索引
	float mWeights[MAX_BONE_INFLUENCE]; // 影响该顶点的骨骼影响权重
};

// 纹理属性结构体
struct Texture {
	unsigned int id; // 纹理id
	std::string type; // 纹理类型
	std::string path; // 纹理路径
};

// 网格类
class Mesh {
public:
	// 网格数据
	std::vector<Vertex> vertices; // 顶点
	std::vector<unsigned int> indices; // 图形(三角形)索引
	std::vector<Texture> textures; // 纹理

public:
	// 构造函数
	Mesh(std::vector<Vertex> _vertices,
		 std::vector<unsigned int> _indices,
		 std::vector<Texture> _textures);

	// 网格绘制
	void draw(Shader& shader);

private:
	// 渲染数据
	unsigned int VAO, VBO, EBO;

	// 渲染网格图形
	void setupMesh();
};

#endif // !MESH_H_
