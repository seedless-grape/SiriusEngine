#include "model.h"

#include <iostream>

unsigned int textureFromFile(const char* path, const std::string& directory, bool gamma) {
	// �����ļ�·��
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	// ��������ͼ����ߺ���ɫͨ��
	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width,
									&height, &nrComponents, 0);

	// ���塢�����������
	unsigned int textureID;
	glGenTextures(1, &textureID);

	if (data) { // �������ͼ����سɹ�
		GLenum format; // OpenGL��ö����
		if (nrComponents == 1) // ��ͨ��
			format = GL_RED;
		else if (nrComponents == 3) // RGB
			format = GL_RGB;
		else if (nrComponents == 4) // RGBA;
			format = GL_RGBA;

		// ���������
		glBindTexture(GL_TEXTURE_2D, textureID);

		// ������������
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
						0, format, GL_UNSIGNED_BYTE, data);

		// ����MipMap
		glGenerateMipmap(GL_TEXTURE_2D);

		// �����������Բ���
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// �ͷŶ�ȡ��ͼƬ�ڴ�
		stbi_image_free(data);
	} else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// ģ��

Model::Model(std::string const& path, std::string _name, bool gamma) :
	Object(_name), gammaCorrection(gamma) { }

Model::Model(glm::vec3 _position, std::string const& path,
			 std::string _name, bool gamma,
			 glm::vec3 _rotaion, glm::vec3 _scale, glm::vec3 _color) :
	Object(_position, _rotaion, _scale, _color, _name), gammaCorrection(gamma) { }

void Model::draw(Renderer& renderer, bool drawCoordinate) {
	renderer.render(*this, drawCoordinate);
}

// ģ����Ⱦ

void Model::loadModel(std::string const& path) {
	// ���س������󡪡�Assimp���ݽṹ�ĸ�����
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate
											 | aiProcess_FlipUVs
											 | aiProcess_CalcTangentSpace);

	// ������صĳ�����������Ϊ�ջ򷵻ص����ݲ�����(mflags)
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	// ��¼ģ���ļ�·��
	directory = path.substr(0, path.find_last_of('/'));

	// �ݹ鴦����
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// �������е���������
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// �ݹ��ӽ��
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// �����㡢���ߺ�����
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		// ������λ��
		glm::vec3 meshVector;
		meshVector.x = mesh->mVertices[i].x;
		meshVector.y = mesh->mVertices[i].y;
		meshVector.z = mesh->mVertices[i].z;
		vertex.position = std::move(meshVector);

		// �����߷���
		if (mesh->HasNormals()) {
			glm::vec3 meshNormal;
			meshNormal.x = mesh->mNormals[i].x;
			meshNormal.y = mesh->mNormals[i].y;
			meshNormal.z = mesh->mNormals[i].z;
			vertex.normal = std::move(meshNormal);
		}

		// ��������ӳ��(ֻʹ�õ�һ����������)
		if (mesh->mTextureCoords[0]) { // ȷ�ϵ�ǰ����������
			// ��������
			glm::vec2 meshTexCoord;
			meshTexCoord.x = mesh->mTextureCoords[0][i].x;
			meshTexCoord.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = std::move(meshTexCoord);
			
			// ���߷���
			glm::vec3 meshTangent;
			meshTangent.x = mesh->mTangents[i].x;
			meshTangent.y = mesh->mTangents[i].y;
			meshTangent.z = mesh->mTangents[i].z;
			vertex.tangent = std::move(meshTangent);

			// �����߷���(���߲������)
			glm::vec3 meshBitangent;
			meshBitangent.x = mesh->mBitangents[i].x;
			meshBitangent.y = mesh->mBitangents[i].y;
			meshBitangent.z = mesh->mBitangents[i].z;
			vertex.bitangent = std::move(meshBitangent);
		} else {
			vertex.texCoords = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	// �����������εĶ���(��������������Ϊ��λ����һ��������)
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		// һ��face.mNumIndices����3����Ӧ����һ�������ε���������
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// �������
	if (mesh->mMaterialIndex >= 0) { // ���ģ�Ͱ������ʰ�
		// �Ӳ��ʲֿ��ж���mesh��Ӧ�Ĳ���
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// ��ȡ�����������ͼ
		std::vector<Texture> diffuseMaps =
			loadMaterialTextures(material, aiTextureType_DIFFUSE,
								 "textureDiffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// ��ȡ���淴�������ͼ
		std::vector<Texture> specularMaps =
			loadMaterialTextures(material, aiTextureType_SPECULAR,
								 "textureSpecular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	
		// ��ȡ������ͼ
		std::vector<Texture> normalMaps =
			loadMaterialTextures(material, aiTextureType_HEIGHT,
								 "textureNormal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		// ��ȡ����λ����ͼ
		std::vector<Texture> heightMaps =
			loadMaterialTextures(material, aiTextureType_AMBIENT,
								 "textureHeight");
	}

	// �����洦�����Ϣ���ڹ���model��mesh
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat,
												 aiTextureType type,
												 std::string typeName) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);

		// ����ģ���Ƿ��Ѿ����ع���
		bool hasLoaded = false;
		for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
			// �����Ѽ���ģ�����飬����⵽�Ѿ����ع�����ֱ�Ӽ�������ظ�����
			if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(texturesLoaded[j]);
				hasLoaded = true;
				break;
			}
		}

		// �����ģ��û�б����ع��������
		if (!hasLoaded) {
			Texture tex;
			tex.id = textureFromFile(str.C_Str(), directory);
			tex.type = typeName;
			tex.path = str.C_Str();
			textures.push_back(tex);
			texturesLoaded.push_back(tex);
		}
	}
	return textures;
}
