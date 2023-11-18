/* ui界面 */

#ifndef GUI_H_
#define GUI_H_

#include "imgui.h"

// 前向声明
class SiriusEngine;

class GUI {
public:
	// 状态机
	bool imguiShowDemo;
	bool imguiShowStyleEditor;
	bool imguiShowMetrics;
	bool imguiShowAbout;
	bool  imguiShowInfo;
	float imguiWindowBgAlpha;

	ImVec2 imguiInfoViewPos;

	// 构造/析构函数
	GUI(SiriusEngine& siriusEngine);
	~GUI();

	// 初始化菜单栏样式
	void initStyle();

	// 渲染
	void render();

private:
	SiriusEngine& siriusEngine;
	void renderSceneInspector();
	void renderExtraView();
	void renderInfoView();
	void renderBuiltInView();

};

#endif // !GUI_H_
