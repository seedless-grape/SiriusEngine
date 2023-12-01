#include "resource_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>

// 静态变量初始化
std::map<std::string, Texture> ResourceManager::textures;
std::map<std::string, Shader> ResourceManager::shaders;

// 载入预设物体名
std::vector<std::string> ResourceManager::presetsObjects = {
	u8"小黄鸭", u8"背景板"
};

Shader ResourceManager::loadShader(const char* vertexFile,
								   const char* fragmentFile,
								   const char* geometryFile,
								   std::string name) {
	shaders[name] = loadShaderFromFile(vertexFile, fragmentFile, geometryFile);
	return shaders[name];
}

bool ResourceManager::hasShader(std::string name) {
	return shaders.find(name) != shaders.end();
}

Shader ResourceManager::getShader(std::string name) {
	return shaders[name];
}

Texture ResourceManager::loadTexture(const char* file, std::string name) {
	textures[name] = loadTextureFromFile(file);	  
	return textures[name];
}

Texture ResourceManager::loadTexture(const std::vector<std::string>& files,
									 std::string name) {
	textures[name] = loadTextureFromFile(files);
	return textures[name];
}

bool ResourceManager::hasTexture(std::string name) {
	return textures.find(name) != textures.end();
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

	// 从文件中加载着色器代码
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

	// 创建着色器对象
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

Texture ResourceManager::loadTextureFromFile(const char* file) {
	// 创建纹理对象
	Texture texture;

	// 加载图片
	texture.generate(file);

	return texture;
}

Texture ResourceManager::loadTextureFromFile(const std::vector<std::string>& files) {
	// 创建纹理对象
	Texture texture;

	// 加载图片
	texture.generate(files);

	return texture;
}
