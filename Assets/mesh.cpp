#include "mesh.h"
#include <iostream>

Mesh::Mesh(std::vector<Vertex> _vertices,
		   std::vector<unsigned int> _indices,
		   std::vector<TextureS> _textures) :
	vertices(_vertices), indices(_indices), textures(_textures) {
	setupMesh();
}

void Mesh::draw(Shader& shader) {
	// ����ӳ��Ŀ��
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	// �����������е���������ӳ��
	for (unsigned int i = 0; i < textures.size(); i++) {
		// �����Ӧ������Ԫ
		glActiveTexture(GL_TEXTURE0 + i);

		// ��ȡ������ź���������
		std::string number;
		std::string name = textures[i].type;
		if (name == "textureDiffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "textureSpecular")
			number = std::to_string(specularNr++);
		else if (name == "textureNormal")
			number = std::to_string(normalNr++);
		else if (name == "textureHeight")
			number = std::to_string(heightNr++);

		// ����GPU��������
		shader.setInt((name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	// ��������
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()),
				   GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// �����������ûع�ΪĬ��ֵ
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() {
	// ����VAO VBO EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// ���Ͷ�������
	glBindVertexArray(VAO);

	// ����VBO����
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
				 &vertices[0], GL_STATIC_DRAW);

	// ����EBO����
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
				 &indices[0], GL_STATIC_DRAW);

	// 0:���Ͷ�������
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// 1:���ͷ��߷���
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, normal));

	// 2:������������
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, texCoords));

	// 3:�������߷���
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, tangent));

	// 4:���͸����߷���
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, bitangent));

	// 5:����Ӱ��ö���Ĺ�������
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex),
						   (void*)offsetof(Vertex, mBoneIDs));

	// 6:����Ӱ��ö���Ĺ�����Ȩ��
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, mWeights));

	// ���
	glBindVertexArray(0);
}

