/* ��ɫ */

#ifndef SHADER_H_
#define SHADER_H_

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

// ��ɫ����
class Shader {
public:
	// ����ID
	unsigned int ID;

public:
	// ��������ȡ��������ɫ��
	Shader() = default;

	// ʹ��/�������
	Shader& use();

	// ����shader
	void compile(const char* vertexSource,
				 const char* fragmentSource,
				 const char* geometrySource = nullptr);

	// uniform���߳���

	// ����ֵ
	void setBool(const char* name, bool value, bool useShader = false);
	void setInt(const char* name, int value, bool useShader = false);
	void setFloat(const char* name, float value, bool useShader = false);

	// ��ά��������
	void setVec2(const char* name, const glm::vec2& value, bool useShader = false);
	void setVec2(const char* name, float x, float y, bool useShader = false);

	// ��ά��������
	void setVec3(const char* name, const glm::vec3& value, bool useShader = false);
	void setVec3(const char* name, float x, float y, float z, bool useShader = false);

	// ��ά��������
	void setVec4(const char* name, const glm::vec4& value, bool useShader = false);
	void setVec4(const char* name, float x, float y, float z, float w, bool useShader = false);

	// ��ά������
	void setMat2(const char* name, const glm::mat2& mat, bool useShader = false);

	// ��ά������
	void setMat3(const char* name, const glm::mat3& mat, bool useShader = false);

	// ��ά������
	void setMat4(const char* name, const glm::mat4 mat, bool useShader = false);

private:
	// ������
	void checkCompileErrors(unsigned int programIdx,
							std::string type);

};

#endif