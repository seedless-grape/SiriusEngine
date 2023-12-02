#include "model.h"

#include "Library/stb_image.h"
#include "resource_manager.h"

#include <iostream>

Model::Model(std::string name, std::string const& path, bool gamma) :
	Object(name) {
	loadModel(path);
}

void Model::draw(Renderer& renderer, Shadow* shadow, bool drawCoordinate, bool gamma) {
	renderer.render(*this, drawCoordinate, gamma);
	for (Mesh& mesh : meshes)
		mesh.draw(renderer.objectShader, shadow);
}

void Model::shadowDraw(Renderer& renderer) {
	renderer.render(*this, false, false);
	for (Mesh& mesh : meshes)
		mesh.shadowDraw(renderer.objectShader);
}

void Model::shadowModelDraw(Renderer& renderer, Shadow* shadow, bool drawCoordinate, bool gamma) {
	renderer.render(*this, drawCoordinate, gamma);
	for (Mesh& mesh : meshes)
		mesh.shadowModelDraw(renderer.objectShader, shadow);
}

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
		// �����Ѽ���ģ�����飬����⵽�Ѿ����ع�����ֱ�Ӽ�������ظ�����
		if (ResourceManager::hasTexture(str.C_Str())) {
			textures.push_back(ResourceManager::getTexture(str.C_Str()));
			hasLoaded = true;
		}

		// �����ģ��û�б����ع��������
		if (!hasLoaded) {
			Texture tex;
			tex = ResourceManager::loadTexture((directory + '/' + str.C_Str()).c_str(), str.C_Str());
			tex.type = typeName;
			tex.path = str.C_Str();
			textures.push_back(tex);
		}
	}
	return textures;
}

ModelRenderer::ModelRenderer(const Shader& shader) : Renderer(shader) {}

void ModelRenderer::render(const Object& object, bool drawCoordinate, bool gamma) {
	// model�任
	glm::mat4 modelMatrix;

	// �������
	if (object.selected && drawCoordinate) {
		this->coordinateShader.use();
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, object.position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotation.x),
								  glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotation.z),
								  glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotation.y),
								  glm::vec3(0.0f, 1.0f, 0.0f));
		this->coordinateShader.setMat4("modelMatrix", modelMatrix);
		this->renderCoordinate();
	}

	this->objectShader.use();
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, object.position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotation.x),
							  glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotation.z),
							  glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotation.y),
							  glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, object.scale);

	this->objectShader.setMat4("modelMatrix", modelMatrix);
	this->objectShader.setVec3("objectColor", object.color);
	this->objectShader.setFloat("shininess", object.shininess);
	this->objectShader.setBool("gamma", gamma);
}