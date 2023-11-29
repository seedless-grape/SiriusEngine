/* 天空盒 */
#ifndef SKYBOX_H_
#define SKYBOX_H_

#include "Core/object.h"
#include "Core/renderer.h"

// 天空盒类
class Skybox : public Object {
public:
	Skybox(std::string _name = "skybox");

	~Skybox() override = default;

	void draw(Renderer& renderer, bool drawCoordinate = false, bool gamma = false) override;
};

// 天空盒渲染类
class SkyboxRenderer : public Renderer {
public:
	SkyboxRenderer(const Shader& shader);

	~SkyboxRenderer();

	// 渲染天空盒
	void render(const Object& object, bool drawCoordinate = false, bool gamma = false) override;

	// 重写渲染更新逻辑
	void updateRenderer(glm::mat4 spaceMatrix);

private:
	unsigned int skyboxVAO;
	void initRenderData() override;
};

#endif // !SKYBOX_H_
