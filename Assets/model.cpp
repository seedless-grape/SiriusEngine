#include "model.h"

#include <iostream>

unsigned int textureFromFile(const char* path, const std::string& directory, bool gamma) {
	// 材质文件路径
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	// 加载纹理图像宽、高和颜色通道
	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width,
									&height, &nrComponents, 0);

	// 定义、生成纹理对象
	unsigned int textureID;
	glGenTextures(1, &textureID);

	if (data) { // 如果纹理图像加载成功
		GLenum format; // OpenGL宏枚举体
		if (nrComponents == 1) // 单通道
			format = GL_RED;
		else if (nrComponents == 3) // RGB
			format = GL_RGB;
		else if (nrComponents == 4) // RGBA;
			format = GL_RGBA;

		// 绑定纹理对象
		glBindTexture(GL_TEXTURE_2D, textureID);

		// 解释纹理属性
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
						0, format, GL_UNSIGNED_BYTE, data);

		// 生成MipMap
		glGenerateMipmap(GL_TEXTURE_2D);

		// 配置纹理属性参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 释放读取的图片内存
		stbi_image_free(data);
	} else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// 模型

Model::Model(std::string const& path, std::string _name, bool gamma) :
	Object(_name), gammaCorrection(gamma) { }

Model::Model(glm::vec3 _position, std::string const& path,
			 std::string _name, bool gamma,
			 glm::vec3 _rotaion, glm::vec3 _scale, glm::vec3 _color) :
	Object(_position, _rotaion, _scale, _color, _name), gammaCorrection(gamma) { }

void Model::draw(Renderer& renderer, bool drawCoordinate) {
	renderer.render(*this, drawCoordinate);
}

// 模型渲染

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
		for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
			// 遍历已加载模型数组，若检测到已经加载过，则直接加入避免重复加载
			if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(texturesLoaded[j]);
				hasLoaded = true;
				break;
			}
		}

		// 如果该模型没有被加载过，则加载
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
