#include "resource_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "Library/stb_image.h"

// ��̬������ʼ��
std::map<std::string, Texture> ResourceManager::textures;
std::map<std::string, Shader> ResourceManager::shaders;

// ����Ԥ��������
std::vector<std::string> ResourceManager::presetsObjects = {
	u8"ľ��"
};

Shader ResourceManager::loadShader(const char* vertexFile,
								   const char* fragmentFile,
								   const char* geometryFile,
								   std::string name) {
	shaders[name] = loadShaderFromFile(vertexFile, fragmentFile, geometryFile);
	return shaders[name];
}

Shader ResourceManager::getShader(std::string name) {
	return shaders[name];
}

Texture ResourceManager::loadTexture(const char* file, bool alpha,
									 std::string name) {
	textures[name] = loadTextureFromFile(file, alpha);
	return textures[name];
}

Texture ResourceManager::getTexture(std::string name) {
	return textures[name];
}

void ResourceManager::clear() {
	for (auto iter : shaders)
		glDeleteProgram(iter.second.ID);
	for (auto iter : textures)
		glDeleteTextures(1, &iter.second.ID);
}



Shader ResourceManager::loadShaderFromFile(const char* vertexFile,
										   const char* fragmentFile,
										   const char* geometryFile) {
	std::cout << "Initializing Shader with paths: " << std::endl;
	std::cout << "  ---> " << vertexFile << std::endl;
	std::cout << "  ---> " << fragmentFile << std::endl;
	if (geometryFile != nullptr)
		std::cout << "  ---> " << geometryFile << std::endl;

	// ���ļ��м�����ɫ������
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;

	vertexCode = getSourceCodeFromPath(vertexFile, "VERTEX");
	fragmentCode = getSourceCodeFromPath(fragmentFile, "FRAGMENT");
	if (geometryFile != nullptr)
		geometryCode = getSourceCodeFromPath(geometryFile, "GEOMETRY");

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();
	const char* geometrySource = geometryCode.c_str();

	// ������ɫ������
	Shader shader;
	shader.compile(vertexSource, fragmentSource,
				   geometryFile != nullptr ? geometrySource : nullptr);

	return shader;
}

std::string ResourceManager::getSourceCodeFromPath(const char* shaderPath,
												   std::string type) {
	std::string shaderCode;
	std::ifstream shaderFile;

	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();

		shaderCode = shaderStream.str();
	} catch (std::ifstream::failure& e) {
		std::cout << "ERROR::SHADER::" << type
			<< "::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		std::cout << e.code() << " | " << e.what() << std::endl;
	}

	return shaderCode;
}

Texture ResourceManager::loadTextureFromFile(const char* file, bool alpha) {
	// �����������
	Texture texture;
	if (alpha) {	// ����������͸����ͨ��
		texture.internalFormat = GL_RGBA;
		texture.imageFormat = GL_RGBA;
	}

	// ����ͼƬ
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height,
									&nrChannels, 0);
	if (data) {
		texture.generate(width, height, data);
	} else {
		std::cout << "Texture failed to load at path: "
			<< file << std::endl;
	}

	// ����
	stbi_image_free(data);

	return texture;
}
