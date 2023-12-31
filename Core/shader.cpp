#include "shader.h"
#include <iostream>

Shader& Shader::use() {
    glUseProgram(this->ID);
    return *this;
}

void Shader::compile(const char* vertexSource,
                     const char* fragmentSource,
                     const char* geometrySource) {
    unsigned int vertex, fragment, geometry;

    // 顶点着色器
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // 片段着色器
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // 几何着色器
    if (geometrySource != nullptr) {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &geometrySource, nullptr);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }

    // 着色程序
    this->ID = glCreateProgram();
    glAttachShader(this->ID, vertex);
    glAttachShader(this->ID, fragment);
    if (geometrySource != nullptr)
        glAttachShader(this->ID, geometry);
    glLinkProgram(this->ID);
    checkCompileErrors(this->ID, "PROGRAM");

    // 回收
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometrySource != nullptr)
        glDeleteShader(geometry);
}

void Shader::setBool(const char* name, bool value, bool useShader) {
    if (useShader)
        this->use();
    glUniform1i(glGetUniformLocation(this->ID, name), value);
}

void Shader::setInt(const char* name, int value, bool useShader) {
    if (useShader)
        this->use();
    glUniform1i(glGetUniformLocation(this->ID, name), value);
}

void Shader::setFloat(const char* name, float value, bool useShader) {
    if (useShader)
        this->use();
    glUniform1f(glGetUniformLocation(this->ID, name), value);
}

void Shader::setVec2(const char* name, const glm::vec2& value, bool useShader) {
    if (useShader)
        this->use();
    glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}

void Shader::setVec2(const char* name, float x, float y, bool useShader) {
    if (useShader)
        this->use();
    glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}

void Shader::setVec3(const char* name, const glm::vec3& value, bool useShader) {
    if (useShader)
        this->use();
    glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}

void Shader::setVec3(const char* name, float x, float y, float z, bool useShader) {
    if (useShader)
        this->use();
    glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}

void Shader::setVec4(const char* name, const glm::vec4& value, bool useShader) {
    if (useShader)
        this->use();
    glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}

void Shader::setVec4(const char* name, float x, float y, float z, float w, bool useShader) {
    if (useShader)
        this->use();
    glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}

void Shader::setMat2(const char* name, const glm::mat2& mat, bool useShader) {
    if (useShader)
        this->use();
    glUniformMatrix2fv(glGetUniformLocation(this->ID, name), 1,
                       false, glm::value_ptr(mat));
}

void Shader::setMat3(const char* name, const glm::mat3& mat, bool useShader) {
    if (useShader)
        this->use();
    glUniformMatrix3fv(glGetUniformLocation(this->ID, name), 1,
                       false, glm::value_ptr(mat));
}

void Shader::setMat4(const char* name, const glm::mat4 mat, bool useShader) {
    if (useShader)
        this->use();
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1,
                       false, glm::value_ptr(mat));
}

void Shader::checkCompileErrors(unsigned int programIdx, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {    // 着色器
        glGetShaderiv(programIdx, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(programIdx, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER::COMPILE : " << type << "\n"
                << infoLog << "\n"
                << "--------------------------------" << std::endl;
        }
    } else {    // 着色程序
        glGetProgramiv(programIdx, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(programIdx, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER::LINK : " << type << "\n"
                << infoLog << "\n"
                << "--------------------------------" << std::endl;
        }
    }

}
