#include "mesh.h"

#include "Core/shadow.h"

Mesh::Mesh(std::vector<Vertex> _vertices,
		   std::vector<unsigned int> _indices,

		   std::vector<Texture> _textures) :

	vertices(_vertices), indices(_indices), textures(_textures) {
	setupMesh();
}

void Mesh::draw(Shader& shader, Shadow* shadow) {
	// 纹理映射目标
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	//unsigned int normalNr = 1;
	//unsigned int heightNr = 1;

	// 将纹理数组中的所有纹理都映射
	unsigned int i;
	for (i = 0; i < textures.size(); i++) {
		// 激活对应的纹理单元
		glActiveTexture(GL_TEXTURE0 + i);

		// 获取纹理序号和纹理类型
		std::string number;
		std::string name = textures[i].type;
		if (name == "textureDiffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "textureSpecular")
			number = std::to_string(specularNr++);
		//else if (name == "textureNormal")
		//	number = std::to_string(normalNr++);
		//else if (name == "textureHeight")
		//	number = std::to_string(heightNr++);

		// 传入GPU，绑定纹理
		shader.setInt((name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].ID);
	}

	// 是否开启阴影
	if (shadow) {
		glActiveTexture(GL_TEXTURE0 + i);
		shader.setInt("shadowMap", i);

		glBindTexture(GL_TEXTURE_2D, shadow->getDepthMap());
	} else {
		shader.setBool("shadowOn", false);
	}


	// ��������
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()),
				   GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// �������������ûع�ΪĬ��ֵ
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::shadowDraw(Shader& shader) {
	// �����������е�����������ӳ��
	for (unsigned int i = 0; i < textures.size(); i++) {
		// ��ȡ������ź���������
		std::string name = textures[i].type;
		if (name == "textureDiffuse") {
			glActiveTexture(GL_TEXTURE0);

			// ������
			glBindTexture(GL_TEXTURE_2D, textures[i].ID);
			break;
		}
	}

	// 绘制网格
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()),
				   GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 将纹理激活设置回归为默认值
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);

}

void Mesh::shadowModelDraw(Shader& shader, Shadow* shadow) {
	// ����ӳ��Ŀ��
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	//unsigned int normalNr = 1;
	//unsigned int heightNr = 1;

	// �����������е�����������ӳ��
	unsigned int i;
	for (i = 0; i < textures.size(); i++) {
		// �����Ӧ��������Ԫ
		glActiveTexture(GL_TEXTURE0 + i);

		// ��ȡ������ź���������
		std::string number;
		std::string name = textures[i].type;
		if (name == "textureDiffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "textureSpecular")
			number = std::to_string(specularNr++);
		//else if (name == "textureNormal")
		//	number = std::to_string(normalNr++);
		//else if (name == "textureHeight")
		//	number = std::to_string(heightNr++);

		// ����GPU��������
		shader.setInt((name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].ID);
	}

	// �󶨴�����Ӱ����
	glActiveTexture(GL_TEXTURE0 + i);
	shader.setInt("shadowMap", i);

	glBindTexture(GL_TEXTURE_2D, shadow->getDepthMap());

	// ��������
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()),
				   GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// �������������ûع�ΪĬ��ֵ
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::setupMesh() {
	// 创建VAO VBO EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// 解释顶点属性
	glBindVertexArray(VAO);

	// 载入VBO数据
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
				 &vertices[0], GL_STATIC_DRAW);

	// 载入EBO数据
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
				 &indices[0], GL_STATIC_DRAW);



	// 0:解释顶点坐标
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// 1:解释法线方向
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, normal));

	// 2:解释纹理坐标
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, texCoords));

	// 3:解释切线方向
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, tangent));

	// 4:解释副切线方向
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, bitangent));

	// 5:解释影响该顶点的骨骼索引
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex),
						   (void*)offsetof(Vertex, mBoneIDs));

	// 6:解释影响该顶点的骨骼的权重
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, mWeights));

	// 解绑
	glBindVertexArray(0);
}

