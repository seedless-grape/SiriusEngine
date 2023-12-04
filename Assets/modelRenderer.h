/* ģ����Ⱦ */

#ifndef MODEL_RENDERER_H_
#define MODEL_RENDERER_H_

#include "Core/renderer.h"


// ģ����Ⱦ��
class ModelRenderer : public Renderer{
public:
	// ����/��������
	ModelRenderer(const Shader &shader);

	~ModelRenderer() override = default;

	// ��Ⱦ
	void render(const Object& object, bool drawCoordinate = true) override;
};

#endif // !MODEL_RENDERER_H_
