/* 立方体物体 */
#include "Core/object.h"
#include "Core/renderer.h"

// 立方体类
class Cube : public Object {
public:
	Cube(std::string _name = "Cube");

	Cube(glm::vec3 _position,
		 glm::vec3 _rotaion = glm::vec3(0.0f),
		 glm::vec3 _scale = glm::vec3(1.0f),
		 glm::vec3 _color = glm::vec3(1.0f),
		 std::string _name = "Cube");

	~Cube() override = default;

	void draw(Renderer& renderer, Shadow* shadow = nullptr, bool drawCoordinate = true, bool gamma = false) override;
};

// 立方体渲染类
class CubeRenderer : public Renderer {
public:
	CubeRenderer(const Shader& shader);

	~CubeRenderer();

	// 渲染立方体
	void render(const Object& object, bool drawCoordinate = true, bool gamma = false) override;

	// 渲染光源立方体
	void render(const PointLight& pointLight, bool gamma = false) override;

private:
	unsigned int cubeVAO;
	void initRenderData() override;
};