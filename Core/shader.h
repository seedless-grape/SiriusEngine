/* 着色 */

#ifndef SHADER_H_
#define SHADER_H_

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

// 着色器类
class Shader {
public:
	// 程序ID
	unsigned int ID;

public:
	// 构造器读取并构造着色器
	Shader() = default;

	// 使用/激活程序
	Shader& use();

	// 编译shader
	void compile(const char* vertexSource,
				 const char* fragmentSource,
				 const char* geometrySource = nullptr);

	// uniform工具程序

	// 基础值
	void setBool(const char* name, bool value, bool useShader = false);
	void setInt(const char* name, int value, bool useShader = false);
	void setFloat(const char* name, float value, bool useShader = false);

	// 二维向量传入
	void setVec2(const char* name, const glm::vec2& value, bool useShader = false);
	void setVec2(const char* name, float x, float y, bool useShader = false);

	// 三维向量传入
	void setVec3(const char* name, const glm::vec3& value, bool useShader = false);
	void setVec3(const char* name, float x, float y, float z, bool useShader = false);

	// 四维向量传入
	void setVec4(const char* name, const glm::vec4& value, bool useShader = false);
	void setVec4(const char* name, float x, float y, float z, float w, bool useShader = false);

	// 二维矩阵传入
	void setMat2(const char* name, const glm::mat2& mat, bool useShader = false);

	// 三维矩阵传入
	void setMat3(const char* name, const glm::mat3& mat, bool useShader = false);

	// 四维矩阵传入
	void setMat4(const char* name, const glm::mat4 mat, bool useShader = false);

private:
	// 错误检测
	void checkCompileErrors(unsigned int programIdx,
							std::string type);

};

#endif