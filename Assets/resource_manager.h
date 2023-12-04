/* ��Դ���� */

#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <map>
#include <string>

#include "Core/texture.h"
#include "Core/shader.h"
#include "Core/object.h"

// ���ز��ʺ���ɫ����Դ
class ResourceManager {
public:
    // ��Դ�洢��
    static std::map<std::string, Shader>  shaders;      // ��ɫ
    static std::map<std::string, Texture> textures;     // ����
    static std::vector<std::string> presetsObjects;     // Ԥ������

    // ���ز�������ɫ��
    static Shader loadShader(const char* vertexFile,
                             const char* fragmentFile,
                             const char* geometryFile,
                             std::string name);

    // ��ȡ��ɫ������
    static Shader getShader(std::string name);

    // ���ز���������
    static Texture loadTexture(const char* file, std::string name);

    static Texture loadTexture(const std::vector<std::string>& files,
                               std::string name);

    // ��ȡ�������
    static Texture getTexture(std::string name);

    // ����������Դ
    static void clear();

private:
    // ���캯��<˽��>
    ResourceManager() = default;

    // ���ļ��м��ز�������ɫ��
    static Shader loadShaderFromFile(const char* vertexFile,
                                     const char* fragmentFile,
                                     const char* geometryFile = nullptr);

    // ��ȡָ��·������ɫ���ַ�������
    static std::string getSourceCodeFromPath(const char* shaderPath,
                                             std::string type);

    // ���ļ��м��ز���������
    static Texture loadTextureFromFile(const char* file);

    static Texture loadTextureFromFile(const std::vector<std::string>& files);
};

#endif //_RESOURCE_MANAGER_H_
