/* ui���� */

#ifndef GUI_H_
#define GUI_H_

#include "imgui.h"

// ǰ������
class SiriusEngine;

class GUI {
public:
	// ״̬��
	bool imguiShowDemo;
	bool imguiShowStyleEditor;
	bool imguiShowMetrics;
	bool imguiShowAbout;
	bool  imguiShowInfo;
	float imguiWindowBgAlpha;

	ImVec2 imguiInfoViewPos;

	// ����/��������
	GUI(SiriusEngine& siriusEngine);
	~GUI();

	// ��ʼ���˵�����ʽ
	void initStyle();

	// ��Ⱦ
	void render();

private:
	SiriusEngine& siriusEngine;
	void renderSceneInspector();
	void renderExtraView();
	void renderInfoView();
	void renderBuiltInView();

};

#endif // !GUI_H_
