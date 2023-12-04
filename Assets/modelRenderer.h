/* 模型渲染 */

#ifndef MODEL_RENDERER_H_
#define MODEL_RENDERER_H_

#include "Core/renderer.h"


// 模型渲染类
class ModelRenderer : public Renderer{
public:
	// 构造/析构函数
	ModelRenderer(const Shader &shader);

	~ModelRenderer() override = default;

	// 渲染
	void render(const Object& object, bool drawCoordinate = true) override;
};

#endif // !MODEL_RENDERER_H_
