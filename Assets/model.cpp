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
	// 加载场景对象——Assimp数据结构的根对象
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate
											 | aiProcess_FlipUVs
											 | aiProcess_CalcTangentSpace);

	// 如果加载的场景对象根结点为空或返回的数据不完整(mflags)
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	// 记录模型文件路径
	directory = path.substr(0, path.find_last_of('/'));

	// 递归处理结点
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// 处理结点中的所有网格
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// 递归子结点
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// 处理顶点、法线和纹理
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		// 处理顶点位置
		glm::vec3 meshVector;
		meshVector.x = mesh->mVertices[i].x;
		meshVector.y = mesh->mVertices[i].y;
		meshVector.z = mesh->mVertices[i].z;
		vertex.position = std::move(meshVector);

		// 处理法线方向
		if (mesh->HasNormals()) {
			glm::vec3 meshNormal;
			meshNormal.x = mesh->mNormals[i].x;
			meshNormal.y = mesh->mNormals[i].y;
			meshNormal.z = mesh->mNormals[i].z;
			vertex.normal = std::move(meshNormal);
		}

		// 处理纹理映射(只使用第一组纹理坐标)
		if (mesh->mTextureCoords[0]) { // 确认当前网格有纹理
			// 纹理坐标
			glm::vec2 meshTexCoord;
			meshTexCoord.x = mesh->mTextureCoords[0][i].x;
			meshTexCoord.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = std::move(meshTexCoord);

			// 切线方向
			glm::vec3 meshTangent;
			meshTangent.x = mesh->mTangents[i].x;
			meshTangent.y = mesh->mTangents[i].y;
			meshTangent.z = mesh->mTangents[i].z;
			vertex.tangent = std::move(meshTangent);

			// 副切线方向(法线叉乘切线)
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

	// 处理构成三角形的顶点(以三个顶点索引为单位构建一个三角形)
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		// 一般face.mNumIndices都是3，对应构成一个三角形的三个顶点
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// 处理材质
	if (mesh->mMaterialIndex >= 0) { // 如果模型包含材质包
		// 从材质仓库中读出mesh对应的材质
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// 读取漫反射材质贴图
		std::vector<Texture> diffuseMaps =
			loadMaterialTextures(material, aiTextureType_DIFFUSE,
								 "textureDiffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// 读取镜面反射材质贴图
		std::vector<Texture> specularMaps =
			loadMaterialTextures(material, aiTextureType_SPECULAR,
								 "textureSpecular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// 读取法线贴图
		std::vector<Texture> normalMaps =
			loadMaterialTextures(material, aiTextureType_HEIGHT,
								 "textureNormal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		// 读取顶点位移贴图
		std::vector<Texture> heightMaps =
			loadMaterialTextures(material, aiTextureType_AMBIENT,
								 "textureHeight");
	}

	// 将上面处理的信息用于构建model的mesh
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat,
												 aiTextureType type,
												 std::string typeName) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);

		// 检查该模型是否已经加载过了
		bool hasLoaded = false;
		// 遍历已加载模型数组，若检测到已经加载过，则直接加入避免重复加载
		if (ResourceManager::hasTexture(str.C_Str())) {
			textures.push_back(ResourceManager::getTexture(str.C_Str()));
			hasLoaded = true;
		}

		// 如果该模型没有被加载过，则加载
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
	// model变换
	glm::mat4 modelMatrix;

	// 坐标绘制
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