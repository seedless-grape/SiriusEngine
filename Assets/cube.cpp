#include "cube.h"
#include "Core/light.h"

Cube::Cube(std::string _name) : Object(_name) {}

Cube::Cube(glm::vec3 _position, glm::vec3 _rotaion,
		   glm::vec3 _scale, glm::vec3 _color, std::string _name) :
	Object(_position, _rotaion, _scale, _color, _name) {}

void Cube::draw(Renderer& renderer, bool drawCoordinate, bool gamma) {
	renderer.render(*this, drawCoordinate, gamma);
}

// 立方体物体渲染

CubeRenderer::CubeRenderer(const Shader& shader) :
    Renderer(shader), cubeVAO(0) {
    this->initRenderData();
}

CubeRenderer::~CubeRenderer() {
    glDeleteVertexArrays(1, &this->cubeVAO);
    glDeleteVertexArrays(1, &this->coordinateVAO);
}

void CubeRenderer::render(const Object& object, bool drawCoordinate, bool gamma) {
    this->objectShader.use();

    // model变换
    glm::mat4 modelMatrix(1.0f);

    modelMatrix = glm::translate(modelMatrix, object.position);  // translate
    modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotation.x),
                              glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotation.z),
                              glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotation.y),
                              glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, object.scale);

    this->objectShader.setMat4("modelMatrix", modelMatrix);
    this->objectShader.setVec3("objectColor", object.color);
    this->objectShader.setInt("material.diffuse", 0);
    this->objectShader.setInt("material.specular", 1);
    this->objectShader.setFloat("material.shininess", object.shininess);
    this->objectShader.setBool("gamma", gamma);

    // 纹理映射
    glActiveTexture(GL_TEXTURE0);
    object.diffuseTexture.bind();

    glActiveTexture(GL_TEXTURE1);
    object.specularTexture.bind();

    // 图形绘制
    glBindVertexArray(this->cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // 坐标绘制
    if (object.selected && drawCoordinate) {
        this->coordinateShader.use();
        modelMatrix = glm::mat4(1.0f);
        // without scale
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
}

void CubeRenderer::render(const PointLight& pointLight, bool gamma) {
    this->lightCubeShader.use();

    // model变换
    glm::mat4 model(1.0f);

    model = glm::translate(model, pointLight.position);
    model = glm::scale(model, glm::vec3(0.1f));
    this->lightCubeShader.setMat4("modelMatrix", model);
    this->lightCubeShader.setVec3("color", pointLight.diffuse);
    this->lightCubeShader.setBool("gamma", gamma);

    // 光源立方体绘制
    glBindVertexArray(this->cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void CubeRenderer::initRenderData() {
    float vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
        -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
        // right face
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  // bottom-left
    };

    // 创建VAO和VBO
    unsigned int VBO;
    glGenVertexArrays(1, &this->cubeVAO);
    glGenBuffers(1, &VBO);

    // 数据流入缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 解释数据属性
    glBindVertexArray(this->cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // 解绑
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}