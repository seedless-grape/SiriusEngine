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
	// åŠ è½½åœºæ™¯å¯¹è±¡â€”â€”Assimpæ•°æ®ç»“æž„çš„æ ¹å¯¹è±¡
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate
											 | aiProcess_FlipUVs
											 | aiProcess_CalcTangentSpace);

	// å¦‚æžœåŠ è½½çš„åœºæ™¯å¯¹è±¡æ ¹ç»“ç‚¹ä¸ºç©ºæˆ–è¿”å›žçš„æ•°æ®ä¸å®Œæ•´(mflags)
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	// è®°å½•æ¨¡åž‹æ–‡ä»¶è·¯å¾„
	directory = path.substr(0, path.find_last_of('/'));

	// é€’å½’å¤„ç†ç»“ç‚¹
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// å¤„ç†ç»“ç‚¹ä¸­çš„æ‰€æœ‰ç½‘æ ¼
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// é€’å½’å­ç»“ç‚¹
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	std::vector<Texture> textures;

	// å¤„ç†é¡¶ç‚¹ã€æ³•çº¿å’Œçº¹ç†
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		// å¤„ç†é¡¶ç‚¹ä½ç½®
		glm::vec3 meshVector;
		meshVector.x = mesh->mVertices[i].x;
		meshVector.y = mesh->mVertices[i].y;
		meshVector.z = mesh->mVertices[i].z;
		vertex.position = meshVector;

		// å¤„ç†æ³•çº¿æ–¹å‘
		if (mesh->HasNormals()) {
			glm::vec3 meshNormal;
			meshNormal.x = mesh->mNormals[i].x;
			meshNormal.y = mesh->mNormals[i].y;
			meshNormal.z = mesh->mNormals[i].z;
			vertex.normal = meshNormal;
		}

		// å¤„ç†çº¹ç†æ˜ å°„(åªä½¿ç”¨ç¬¬ä¸€ç»„çº¹ç†åæ ‡)
		if (mesh->mTextureCoords[0]) { // ç¡®è®¤å½“å‰ç½‘æ ¼æœ‰çº¹ç†
			// çº¹ç†åæ ‡
			glm::vec2 meshTexCoord;
			meshTexCoord.x = mesh->mTextureCoords[0][i].x;
			meshTexCoord.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = meshTexCoord;

			glm::vec3 meshTangent;
			meshTangent.x = mesh->mTangents[i].x;
			meshTangent.y = mesh->mTangents[i].y;
			meshTangent.z = mesh->mTangents[i].z;
			vertex.tangent = meshTangent;

			// å‰¯åˆ‡çº¿æ–¹å‘(æ³•çº¿å‰ä¹˜åˆ‡çº¿)
			glm::vec3 meshBitangent;
			meshBitangent.x = mesh->mBitangents[i].x;
			meshBitangent.y = mesh->mBitangents[i].y;
			meshBitangent.z = mesh->mBitangents[i].z;
			vertex.bitangent = meshBitangent;
		} else {
			vertex.texCoords = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	// å¤„ç†æž„æˆä¸‰è§’å½¢çš„é¡¶ç‚¹(ä»¥ä¸‰ä¸ªé¡¶ç‚¹ç´¢å¼•ä¸ºå•ä½æž„å»ºä¸€ä¸ªä¸‰è§’å½¢)
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		// ä¸€èˆ¬face.mNumIndiceséƒ½æ˜¯3ï¼Œå¯¹åº”æž„æˆä¸€ä¸ªä¸‰è§’å½¢çš„ä¸‰ä¸ªé¡¶ç‚¹
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// å¤„ç†æè´¨
	if (mesh->mMaterialIndex >= 0) { // å¦‚æžœæ¨¡åž‹åŒ…å«æè´¨åŒ…
		// ä»Žæè´¨ä»“åº“ä¸­è¯»å‡ºmeshå¯¹åº”çš„æè´¨
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


		// è¯»å–æ¼«åå°„æè´¨è´´å›¾
		std::vector<Texture> diffuseMaps =

			loadMaterialTextures(material, aiTextureType_DIFFUSE,
								 "textureDiffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());


		// è¯»å–é•œé¢åå°„æè´¨è´´å›¾
		std::vector<Texture> specularMaps =

			loadMaterialTextures(material, aiTextureType_SPECULAR,
								 "textureSpecular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// ¶ÁÈ¡·¨ÏßÌùÍ¼
		std::vector<Texture> normalMaps =

			loadMaterialTextures(material, aiTextureType_HEIGHT,
								 "textureNormal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    
		// ¶ÁÈ¡´Ö²Ú¶ÈÌùÍ¼
		std::vector<Texture> heightMaps =
			loadMaterialTextures(material, aiTextureType_SHININESS,
								 "textureRough");
	}

	// å°†ä¸Šé¢å¤„ç†çš„ä¿¡æ¯ç”¨äºŽæž„å»ºmodelçš„mesh
	return Mesh(vertices, indices, textures);
}


std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat,
												 aiTextureType type,
												 std::string typeName) {
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);

		// æ£€æŸ¥è¯¥æ¨¡åž‹æ˜¯å¦å·²ç»åŠ è½½è¿‡äº†
		bool hasLoaded = false;

		// ±éÀúÒÑ¼ÓÔØÄ£ÐÍÊý×é£¬Èô¼ì²âµ½ÒÑ¾­¼ÓÔØ¹ý£¬ÔòÖ±½Ó¼ÓÈë±ÜÃâÖØ¸´¼ÓÔØ
		if (ResourceManager::hasTexture(str.C_Str())) {
			textures.push_back(ResourceManager::getTexture(str.C_Str()));
			hasLoaded = true;
		}

		// å¦‚æžœè¯¥æ¨¡åž‹æ²¡æœ‰è¢«åŠ è½½è¿‡ï¼Œåˆ™åŠ è½½
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
	// model±ä»»
	glm::mat4 modelMatrix;

	// ×ø±ê»æÖÆ
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