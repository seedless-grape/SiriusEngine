#include "gui.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Engine/sirius_engine.h"
#include "Assets/resource_manager.h"
#include "Core/object.h"
#include "Assets/loadPresets.h"

#include <iostream>
#include <sstream>

// 常量
const char* IMGUI_FONT_FAMILY = "Resources/fonts/SimHei.ttf";
const float IMGUI_FONT_SIZE = 16.0f;

const float IMGUI_FRAME_CORNER_SIZE = 2.0f;
const float IMGUI_VIEW_MARGIN = 20.0f;

const float IMGUI_DRAG_SPEED_SUPER_SLOW = 0.001f;
const float IMGUI_DRAG_SPEED_SLOW = 0.005f;
const float IMGUI_DRAG_SPEED_MEDIUM = 0.01f;
const float IMGUI_DRAG_SPEED_FAST = 0.1f;
const float IMGUI_DRAG_SPEED_SUPER_FAST = 1.0f;

const ImVec2 IMGUI_ITEM_SPACING = ImVec2(8.0f, 8.0f);
const ImVec2 IMGUI_INFO_VIEW_SIZE = ImVec2(200.0f, 58.0f);

// 前向声明
float ImGuiGetItemWidth();
void HelpMarker(const char* desc);

GUI::GUI(SiriusEngine& engine)
    : siriusEngine(engine),
    imguiShowDemo(false), imguiShowStyleEditor(false), imguiShowMetrics(false),
    imguiShowAbout(false), imguiShowInfo(true), imguiWindowBgAlpha(0.9f) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(siriusEngine.window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    imguiInfoViewPos = ImVec2(siriusEngine.width - (IMGUI_INFO_VIEW_SIZE.x + IMGUI_VIEW_MARGIN), IMGUI_VIEW_MARGIN);
}

GUI::~GUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::initStyle() {
    ImGuiIO& io = ImGui::GetIO();

    // 字体
    const float dpi = 2.0f;
    ImGui::GetIO().FontGlobalScale = 1 / dpi;
    ImGui::GetIO().Fonts->AddFontFromFileTTF(IMGUI_FONT_FAMILY, IMGUI_FONT_SIZE * dpi, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());

    // 菜单栏样式
    ImGui::GetStyle().FrameRounding = IMGUI_FRAME_CORNER_SIZE;
    ImGui::GetStyle().ItemSpacing = IMGUI_ITEM_SPACING;
}

void GUI::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    this->renderSceneInspector();
    this->renderExtraView();
    this->renderInfoView();
    this->renderBuiltInView();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::renderSceneInspector() {
    ImGui::SetNextWindowBgAlpha(imguiWindowBgAlpha);
    ImGui::Begin(u8"主菜单");
    {
        // 物体设置菜单
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.55f, 0.53f, 0.31f, 1.0f));  // 物体设置选项栏的颜色begin
        if (ImGui::CollapsingHeader(u8"物体设置", ImGuiTreeNodeFlags_DefaultOpen)) {
          
            ImGui::PushID(u8"物体设置");

            // 当前选中的物体
            int& selectIndex = siriusEngine.currentSelectedObjectIndex;

            if (selectIndex >= 0) {
                Object* currentObject = siriusEngine.sceneObjects[selectIndex];

                // 当前选中物体选择框
                ImGui::PushItemWidth(ImGui::GetWindowSize().x - 100.0f);
                std::stringstream ss;
                ss << selectIndex + 1 << ". " << currentObject->name;
                if (ImGui::BeginCombo("##sceneObjectCombo", ss.str().c_str())) {
                    for (unsigned int i = 0; i < siriusEngine.sceneObjects.size(); ++i) {
                        bool selected = (i == siriusEngine.currentSelectedObjectIndex);
                        std::stringstream ss;
                        ss << i + 1 << ". " << siriusEngine.sceneObjects[i]->name;
                        if (ImGui::Selectable(ss.str().c_str(), selected)) {
                            siriusEngine.currentSelectedObjectIndex = i;
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();

                ImGui::SameLine();

                // 是否显示物体
                ImGui::Checkbox("##enableObject", &currentObject->enabled);

                ImGui::SameLine();

                // 重置物体
                if (ImGui::Button(u8"重置")) {
                    currentObject->reset();
                }

                ImGui::Separator();

                // 状态数值
                ImGui::DragFloat3(u8"位置", glm::value_ptr(currentObject->position), IMGUI_DRAG_SPEED_SLOW, 0.0f, 0.0f, "%.3f");
                ImGui::DragFloat3(u8"旋转", glm::value_ptr(currentObject->rotation), IMGUI_DRAG_SPEED_FAST, 0.0f, 0.0f, "%.2f");
                ImGui::DragFloat3(u8"缩放", glm::value_ptr(currentObject->scale), IMGUI_DRAG_SPEED_MEDIUM, 0.0f, 0.0f, "%.2f");
                ImGui::ColorEdit3(u8"颜色", glm::value_ptr(currentObject->color));
                
                // 是否显示坐标轴
                ImGui::Checkbox(u8"坐标轴", &siriusEngine.isObjectCoordinateShown);

                ImGui::SameLine();

                // 删除当前物体
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.44f, 0.17f, 0.17f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.27f, 0.27f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.85f, 0.31f, 0.31f, 1.0f));
                ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2.0f + 80.0f, ImGui::GetCursorPos().y));

                if (ImGui::Button(u8"删除物体", ImVec2(80.0f, 0.0f))) {
                    if (selectIndex >= 0) {
                        delete siriusEngine.sceneObjects[selectIndex];
                        siriusEngine.sceneObjects.erase(siriusEngine.sceneObjects.begin() + selectIndex);
                        if (selectIndex == siriusEngine.sceneObjects.size())
                            --selectIndex;
                    }
                }
                ImGui::PopStyleColor(3);
            }

            ImGui::Separator();

            // 添加指定物体
            int& addIndex = siriusEngine.currentAddObjectIndex;
            ImGui::PushItemWidth(ImGui::GetWindowSize().x - 120.0f);
            if (ImGui::BeginCombo("##presetsObjectCombo", ResourceManager::presetsObjects[addIndex].c_str())) {
                for (unsigned int i = 0; i < ResourceManager::presetsObjects.size(); ++i) {
                    bool selected = (i == siriusEngine.currentAddObjectIndex);
                    if (ImGui::Selectable(ResourceManager::presetsObjects[i].c_str(), selected)) {
                        siriusEngine.currentAddObjectIndex = i;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();

            ImGui::SameLine();

            ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2.0f + 80.0f, ImGui::GetCursorPos().y));
            if (ImGui::Button(u8"添加物体", ImVec2(80.0f, 0.0f))) {
                Object* obj = LoadPresets::loadModel((modelType)addIndex, ResourceManager::presetsObjects[addIndex]);
                siriusEngine.sceneObjects.push_back(obj);
                siriusEngine.currentSelectedObjectIndex = siriusEngine.sceneObjects.size() - 1;
            }

            ImGui::PopID();
        }
        ImGui::PopStyleColor(1);    // 物体设置选项栏的颜色end

        // 相机设置选项栏
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.55f, 0.53f, 0.31f, 1.0f));  // 相机设置选项栏的颜色begin
        if (ImGui::CollapsingHeader(u8"相机设置", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID(u8"相机设置");

            // 相机状态数值
            ImGui::DragFloat3(u8"位置", glm::value_ptr(siriusEngine.camera.position), IMGUI_DRAG_SPEED_SLOW);
            ImGui::PushItemWidth(ImGuiGetItemWidth());
            ImGui::DragFloat(u8"偏航角", &siriusEngine.camera.yaw, IMGUI_DRAG_SPEED_FAST, 0.0f, 0.0f, "%.1f");
            ImGui::SameLine();
            ImGui::DragFloat(u8"俯仰角", &siriusEngine.camera.pitch, IMGUI_DRAG_SPEED_FAST, 0.0f, 0.0f, "%.1f");
            ImGui::PopItemWidth();

            ImGui::Text(u8"相机前朝向: (%.3f, %.3f, %.3f)",
                        siriusEngine.camera.front.x, siriusEngine.camera.front.y,
                        siriusEngine.camera.front.z);

            // 视图状态数值
            ImGui::Separator();

            if (siriusEngine.camera.projectionType == PERSPECTIVE) {
                ImGui::SliderFloat(u8"视场角", &siriusEngine.camera.zoom, ZOOM_MIN, ZOOM_MAX, "%.1f");
            } else {
                ImGui::SliderFloat(u8"视野范围", &siriusEngine.camera.ortho, ORTHO_MIN, ORTHO_MAX, "%.2f");
            }

            ImGui::PushItemWidth(ImGuiGetItemWidth());
            ImGui::DragFloat(u8"近平面", &siriusEngine.camera.near, IMGUI_DRAG_SPEED_FAST, 0.0f, 0.0f, "%.1f");
            ImGui::SameLine();
            ImGui::DragFloat(u8"近平面", &siriusEngine.camera.far, IMGUI_DRAG_SPEED_FAST, 0.0f, 0.0f, "%.1f");
            ImGui::PopItemWidth();
            ImGui::SameLine();
            HelpMarker(u8"近 / 远平面");

            ImGui::RadioButton(u8"透视", (int*)&siriusEngine.camera.projectionType, 0);
            ImGui::SameLine();
            ImGui::RadioButton(u8"正交", (int*)&siriusEngine.camera.projectionType, 1);

            ImGui::Separator();

            // 控制状态数值
            ImGui::PushItemWidth(ImGuiGetItemWidth());
            ImGui::DragFloat(u8"相机速度", &siriusEngine.camera.movementSpeed, IMGUI_DRAG_SPEED_MEDIUM, 0.0f, 0.0f, "%.2f");
            ImGui::SameLine();
            ImGui::DragFloat(u8"鼠标灵敏度", &siriusEngine.camera.mouseSensitivity, IMGUI_DRAG_SPEED_MEDIUM, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();

            ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2.0f - 50.0f, ImGui::GetCursorPos().y));
            if (ImGui::Button(u8"重置相机状态")) {
                siriusEngine.camera.reset();
            }

            ImGui::PopID();
        }
        ImGui::PopStyleColor(1);    // 相机设置选项栏的颜色end

        // 光照设置选项栏
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.55f, 0.53f, 0.31f, 1.0f));  // 光照设置选项栏的颜色begin
        if (ImGui::CollapsingHeader(u8"光照设置", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID(u8"光照设置");

			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar(u8"光照样式条", tab_bar_flags)) {
				int& selectIndex = siriusEngine.currentSelectedPointLightIndex;
				if (selectIndex >= 0) {
                    PointLight* currentPointLight = siriusEngine.scenePointLights[selectIndex];
					
                    // 当前选中点光源选择框
                    if (ImGui::BeginTabItem(u8"点光源")) {
						ImGui::PushItemWidth(ImGui::GetWindowSize().x - 100.0f);
						std::stringstream ss;
						ss << selectIndex + 1 << u8". 点光源";
						if (ImGui::BeginCombo("##scenePointLightsCombo", ss.str().c_str())) {
							for (unsigned int i = 0; i < siriusEngine.scenePointLights.size(); ++i) {
								bool selected = (i == siriusEngine.currentSelectedPointLightIndex);
								std::stringstream ss;
								ss << i + 1 << u8". 点光源";
								if (ImGui::Selectable(ss.str().c_str(), selected)) {
									siriusEngine.currentSelectedPointLightIndex = i;
								}
							}
							ImGui::EndCombo();
						}
						ImGui::PopItemWidth();

						ImGui::SameLine();
						ImGui::Checkbox("##enablePointLight", &currentPointLight->enabled);
						ImGui::SameLine();
						if (ImGui::Button(u8"重置")) {
							currentPointLight->reset();
						}

                        // 添加点光源
                        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2.0f - 120.0f, ImGui::GetCursorPos().y));
                        if (ImGui::Button(u8"添加点光源", ImVec2(100.0f, 0.0f))) {
                            PointLight* light = LoadPresets::loadPointLight();
                            siriusEngine.scenePointLights.push_back(light);
                            siriusEngine.currentSelectedPointLightIndex = siriusEngine.scenePointLights.size() - 1;
                        }

                        ImGui::SameLine();

                        // 删除当前点光源
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.44f, 0.17f, 0.17f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.27f, 0.27f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.85f, 0.31f, 0.31f, 1.0f));
                        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2.0f + 20.0f, ImGui::GetCursorPos().y));

                        if (ImGui::Button(u8"删除点光源", ImVec2(100.0f, 0.0f))) {
                            if (selectIndex >= 0) {
                                delete siriusEngine.scenePointLights[selectIndex];
                                siriusEngine.scenePointLights.erase(siriusEngine.scenePointLights.begin() + selectIndex);
                                if (selectIndex == siriusEngine.scenePointLights.size())
                                    --selectIndex;
                            }
                        }
                        ImGui::PopStyleColor(3);

						ImGui::Separator();

						ImGui::ColorEdit3(u8"环境光", glm::value_ptr(currentPointLight->ambient));
						ImGui::ColorEdit3(u8"漫反射", glm::value_ptr(currentPointLight->diffuse));
						ImGui::ColorEdit3(u8"镜面反射", glm::value_ptr(currentPointLight->specular));

						ImGui::DragFloat3(u8"光衰减因子", glm::value_ptr(currentPointLight->attenuationFactors), IMGUI_DRAG_SPEED_SUPER_SLOW, 0.0f, 1.0f);
						ImGui::SameLine();
						HelpMarker(u8"光衰减因子系数包含: 常数项，一次项，二次项");

						ImGui::Separator();

						ImGui::DragFloat3(u8"位置", glm::value_ptr(currentPointLight->position), IMGUI_DRAG_SPEED_SLOW);

						ImGui::EndTabItem();
					}
                }

                if (ImGui::BeginTabItem(u8"定向光")) {
                    DirLight& dirLight = siriusEngine.dirLight;

                    ImGui::Text(u8"显示定向光");
                    ImGui::SameLine();
                    ImGui::Checkbox("##enableDirectionalLight", &dirLight.enabled);
                    ImGui::SameLine();
                    if (ImGui::Button(u8"重置")) {
                        dirLight.reset();
                    }

                    ImGui::Separator();

                    ImGui::ColorEdit3(u8"环境光", glm::value_ptr(dirLight.ambient));
                    ImGui::ColorEdit3(u8"漫反射", glm::value_ptr(dirLight.diffuse));
                    ImGui::ColorEdit3(u8"镜面反射", glm::value_ptr(dirLight.specular));

                    ImGui::Separator();

                    ImGui::DragFloat3(u8"方向", glm::value_ptr(dirLight.direction), IMGUI_DRAG_SPEED_SLOW);
                    ImGui::SliderFloat(u8"半径", &dirLight.radius, DIR_RADIUS_MIN, DIR_RADIUS_MAX, "%.1f");

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::PopID();
        }
        ImGui::PopStyleColor(1);    // 光照设置选项栏的颜色end
    }
    ImGui::End();
}

void GUI::renderExtraView() {
    // Extra Options
    // ---------------------------------------
    ImGui::SetNextWindowBgAlpha(imguiWindowBgAlpha);
    ImGui::Begin("Extra Options");
    {
        // GLFW
        if (ImGui::CollapsingHeader("GLFW", ImGuiTreeNodeFlags_DefaultOpen)) {
            // window size
            ImGui::Text("Window Size: ");
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGuiGetItemWidth());
            ImGui::InputInt("##ScreenWidth", (int*)&siriusEngine.width, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::SameLine();
            ImGui::Text(" x ");
            ImGui::SameLine();
            ImGui::InputInt("##ScreenHeight", (int*)&siriusEngine.height, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopItemWidth();

            ImGui::Checkbox("FPS", &imguiShowInfo);
            ImGui::SameLine();

            // mouse control
            ImGui::Checkbox("Mouse", &siriusEngine.isMouseControlOn);
            ImGui::SameLine();
            ImGui::Checkbox("Scroll", &siriusEngine.isScrollControlOn);
            ImGui::SameLine();
            HelpMarker("[Rotate Object] Hold CTRL + Hold Mouse Left Button\n"
                       "[Look Around] Hold Mouse Right Button\n"
                       "[Move Camera] Press W/A/S/D/Shift/Space\n"
                       "[Change Fov] Hold Mouse Right Button + Scroll");
        }

        // OpenGL
        if (ImGui::CollapsingHeader("OpenGL", ImGuiTreeNodeFlags_DefaultOpen)) {
            // version
            int majorVersion, minorVersion, revisionNumber;
            glfwGetVersion(&majorVersion, &minorVersion, &revisionNumber);
            ImGui::Text("Version: %d.%d.%d", majorVersion, minorVersion, revisionNumber);
            // color
            ImGui::ColorEdit3("Clear Color", glm::value_ptr(siriusEngine.clearColor));
            // 选项
            ImGui::Checkbox(u8"深度测试", &siriusEngine.isDepthTestOn);
            ImGui::SameLine();
            ImGui::Checkbox(u8"模板测试", &siriusEngine.isStencilTestOn);
            ImGui::SameLine();
            ImGui::Checkbox(u8"背面剔除", &siriusEngine.isFaceCullingOn);
            const char* postProcessingNames[postProcessingCount] = {u8"无特效", u8"反相", u8"灰度", u8"锐化", u8"模糊", u8"边缘"};
            const char* postProcessingName = (siriusEngine.postProcessing >= 0 && siriusEngine.postProcessing < postProcessingCount) ?
                postProcessingNames[siriusEngine.postProcessing] : u8"未知";
            ImGui::SliderInt(u8"后期特效", &siriusEngine.postProcessing, 0, postProcessingCount - 1, postProcessingName);
            ImGui::Checkbox(u8"MSAA抗锯齿", &siriusEngine.isMSAAOn);
            ImGui::SameLine();
            ImGui::Checkbox(u8"Gamma矫正", &siriusEngine.isGammaOn);

            ImGui::Separator();
            
            ImGui::Checkbox(u8"bias优化硬阴影", &siriusEngine.isBiasShadowOn);
            ImGui::SameLine();
            ImGui::Checkbox(u8"正面剔除优化硬阴影", &siriusEngine.isCullShadowOn);

        }

        // ImGui
        if (ImGui::CollapsingHeader("ImGui", ImGuiTreeNodeFlags_DefaultOpen)) {
            // version
            ImGui::Text("Version: %s", ImGui::GetVersion());

            ImGui::SliderFloat("Bg Alpha", &imguiWindowBgAlpha, 0.0f, 1.0f, "%.2f");

            // built-in views
            ImGui::Checkbox("Demo View", &imguiShowDemo);
            ImGui::SameLine();
            ImGui::Checkbox("Style Editor", &imguiShowStyleEditor);
            ImGui::Checkbox("Metrics View", &imguiShowMetrics);
            ImGui::SameLine();
            ImGui::Checkbox("About ImGui", &imguiShowAbout);
        }
    }
    ImGui::End();
}

void GUI::renderInfoView() {
    if (imguiShowInfo) {
        ImGui::SetNextWindowSize(IMGUI_INFO_VIEW_SIZE);
        ImGui::SetNextWindowPos(imguiInfoViewPos);
        ImGui::SetNextWindowBgAlpha(imguiWindowBgAlpha);
        ImGui::Begin("Info View", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

        if (ImGui::BeginTable("##InfoTable", 2)) {
            // row 0
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("fps");
            ImGui::SameLine();
            HelpMarker("Frames Per Second");
            ImGui::TableNextColumn();
            ImGui::Text(":  %d", (int)ImGui::GetIO().Framerate);

            // row 1
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("ms/frame");
            ImGui::TableNextColumn();
            ImGui::Text(":  %.2f ms", 1000 / ImGui::GetIO().Framerate);

            ImGui::EndTable();
        }

        ImGui::End();
    }
}

void GUI::renderBuiltInView() {
    if (imguiShowDemo)
        ImGui::ShowDemoWindow(&imguiShowDemo);
    if (imguiShowStyleEditor)
        ImGui::ShowStyleEditor();
    if (imguiShowMetrics)
        ImGui::ShowMetricsWindow(&imguiShowMetrics);
    if (imguiShowAbout)
        ImGui::ShowAboutWindow(&imguiShowAbout);
}

float ImGuiGetItemWidth() {
    return ImGui::GetWindowSize().x / 4.65f;
}

void HelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

