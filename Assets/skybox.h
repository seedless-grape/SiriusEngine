/* ��պ� */
#ifndef SKYBOX_H_
#define SKYBOX_H_

#include "Core/object.h"
#include "Core/renderer.h"

// ��պ���
class Skybox : public Object {
public:
	Skybox(std::string _name = "skybox");

	~Skybox() override = default;

	void draw(Renderer& renderer, bool drawCoordinate = false, bool gamma = false) override;
};

// ��պ���Ⱦ��
class SkyboxRenderer : public Renderer {
public:
	SkyboxRenderer(const Shader& shader);

	~SkyboxRenderer();

	// ��Ⱦ��պ�
	void render(const Object& object, bool drawCoordinate = false, bool gamma = false) override;

	// ��д��Ⱦ�����߼�
	void updateRenderer(glm::mat4 spaceMatrix);

private:
	unsigned int skyboxVAO;
	void initRenderData() override;
};

#endif // !SKYBOX_H_
