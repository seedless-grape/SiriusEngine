#include "modelRenderer.h"
#include "Core/object.h"

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
    this->objectShader.setFloat("shininess", object.shininess);
    this->objectShader.setBool("gamma", gamma);
}