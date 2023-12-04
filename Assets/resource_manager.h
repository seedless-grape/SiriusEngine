/* 资源管理 */

#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <map>
#include <string>

#include "Core/texture.h"
#include "Core/shader.h"
#include "Core/object.h"

// 加载材质和着色器资源
class ResourceManager {
public:
    // 资源存储器
    static std::map<std::string, Shader>  shaders;      // 着色
    static std::map<std::string, Texture> textures;     // 材质
    static std::vector<std::string> presetsObjects;     // 预设物体

    // 加载并生成着色器
    static Shader loadShader(const char* vertexFile,
                             const char* fragmentFile,
                             const char* geometryFile,
                             std::string name);

    // 获取着色器对象
    static Shader getShader(std::string name);

    // 加载并生成纹理
    static Texture loadTexture(const char* file, std::string name);

    static Texture loadTexture(const std::vector<std::string>& files,
                               std::string name);

    // 获取纹理对象
    static Texture getTexture(std::string name);

    // 回收所有资源
    static void clear();

private:
    // 构造函数<私有>
    ResourceManager() = default;

    // 从文件中加载并生成着色器
    static Shader loadShaderFromFile(const char* vertexFile,
                                     const char* fragmentFile,
                                     const char* geometryFile = nullptr);

    // 获取指定路径的着色器字符串编码
    static std::string getSourceCodeFromPath(const char* shaderPath,
                                             std::string type);

    // 从文件中加载并生成纹理
    static Texture loadTextureFromFile(const char* file);

    static Texture loadTextureFromFile(const std::vector<std::string>& files);
};

#endif //_RESOURCE_MANAGER_H_
