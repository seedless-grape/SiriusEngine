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

// ����
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

// ǰ������
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

    // ����
    const float dpi = 2.0f;
    ImGui::GetIO().FontGlobalScale = 1 / dpi;
    ImGui::GetIO().Fonts->AddFontFromFileTTF(IMGUI_FONT_FAMILY, IMGUI_FONT_SIZE * dpi, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());

    // �˵�����ʽ
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
    ImGui::Begin(u8"���˵�");
    {
        // �������ò˵�
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.55f, 0.53f, 0.31f, 1.0f));  // ��������ѡ��������ɫbegin
        if (ImGui::CollapsingHeader(u8"��������", ImGuiTreeNodeFlags_DefaultOpen)) {
          
            ImGui::PushID(u8"��������");

            // ��ǰѡ�е�����
            int& selectIndex = siriusEngine.currentSelectedObjectIndex;

            if (selectIndex >= 0) {
                Object* currentObject = siriusEngine.sceneObjects[selectIndex];

                // ��ǰѡ������ѡ���
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

                // �Ƿ���ʾ����
                ImGui::Checkbox("##enableObject", &currentObject->enabled);

                ImGui::SameLine();

                // ��������
                if (ImGui::Button(u8"����")) {
                    currentObject->reset();
                }

                ImGui::Separator();

                // ״̬��ֵ
                ImGui::DragFloat3(u8"λ��", glm::value_ptr(currentObject->position), IMGUI_DRAG_SPEED_SLOW, 0.0f, 0.0f, "%.3f");
                ImGui::DragFloat3(u8"��ת", glm::value_ptr(currentObject->rotation), IMGUI_DRAG_SPEED_FAST, 0.0f, 0.0f, "%.2f");
                ImGui::DragFloat3(u8"����", glm::value_ptr(currentObject->scale), IMGUI_DRAG_SPEED_MEDIUM, 0.0f, 0.0f, "%.2f");
                ImGui::ColorEdit3(u8"��ɫ", glm::value_ptr(currentObject->color));
                
                // �Ƿ���ʾ������
                ImGui::Checkbox(u8"������", &siriusEngine.isObjectCoordinateShown);

                ImGui::SameLine();

                // ɾ����ǰ����
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.44f, 0.17f, 0.17f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.27f, 0.27f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.85f, 0.31f, 0.31f, 1.0f));
                ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2.0f + 80.0f, ImGui::GetCursorPos().y));

                if (ImGui::Button(u8"ɾ������", ImVec2(80.0f, 0.0f))) {
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

            // ���ָ������
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
            if (ImGui::Button(u8"�������", ImVec2(80.0f, 0.0f))) {
                Object* obj = LoadPresets::loadModel((modelType)addIndex, ResourceManager::presetsObjects[addIndex]);
                siriusEngine.sceneObjects.push_back(obj);
                siriusEngine.currentSelectedObjectIndex = siriusEngine.sceneObjects.size() - 1;
            }

            ImGui::PopID();
        }
        ImGui::PopStyleColor(1);    // ��������ѡ��������ɫend

        // �������ѡ����
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.55f, 0.53f, 0.31f, 1.0f));  // �������ѡ��������ɫbegin
        if (ImGui::CollapsingHeader(u8"�������", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID(u8"�������");

            // ���״̬��ֵ
            ImGui::DragFloat3(u8"λ��", glm::value_ptr(siriusEngine.camera.position), IMGUI_DRAG_SPEED_SLOW);
            ImGui::PushItemWidth(ImGuiGetItemWidth());
            ImGui::DragFloat(u8"ƫ����", &siriusEngine.camera.yaw, IMGUI_DRAG_SPEED_FAST, 0.0f, 0.0f, "%.1f");
            ImGui::SameLine();
            ImGui::DragFloat(u8"������", &siriusEngine.camera.pitch, IMGUI_DRAG_SPEED_FAST, 0.0f, 0.0f, "%.1f");
            ImGui::PopItemWidth();

            ImGui::Text(u8"���ǰ����: (%.3f, %.3f, %.3f)",
                        siriusEngine.camera.front.x, siriusEngine.camera.front.y,
                        siriusEngine.camera.front.z);

            // ��ͼ״̬��ֵ
            ImGui::Separator();

            if (siriusEngine.camera.projectionType == PERSPECTIVE) {
                ImGui::SliderFloat(u8"�ӳ���", &siriusEngine.camera.zoom, ZOOM_MIN, ZOOM_MAX, "%.1f");
            } else {
                ImGui::SliderFloat(u8"��Ұ��Χ", &siriusEngine.camera.ortho, ORTHO_MIN, ORTHO_MAX, "%.2f");
            }

            ImGui::PushItemWidth(ImGuiGetItemWidth());
            ImGui::DragFloat(u8"��ƽ��", &siriusEngine.camera.near, IMGUI_DRAG_SPEED_FAST, 0.0f, 0.0f, "%.1f");
            ImGui::SameLine();
            ImGui::DragFloat(u8"��ƽ��", &siriusEngine.camera.far, IMGUI_DRAG_SPEED_FAST, 0.0f, 0.0f, "%.1f");
            ImGui::PopItemWidth();
            ImGui::SameLine();
            HelpMarker(u8"�� / Զƽ��");

            ImGui::RadioButton(u8"͸��", (int*)&siriusEngine.camera.projectionType, 0);
            ImGui::SameLine();
            ImGui::RadioButton(u8"����", (int*)&siriusEngine.camera.projectionType, 1);

            ImGui::Separator();

            // ����״̬��ֵ
            ImGui::PushItemWidth(ImGuiGetItemWidth());
            ImGui::DragFloat(u8"����ٶ�", &siriusEngine.camera.movementSpeed, IMGUI_DRAG_SPEED_MEDIUM, 0.0f, 0.0f, "%.2f");
            ImGui::SameLine();
            ImGui::DragFloat(u8"���������", &siriusEngine.camera.mouseSensitivity, IMGUI_DRAG_SPEED_MEDIUM, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();

            ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2.0f - 50.0f, ImGui::GetCursorPos().y));
            if (ImGui::Button(u8"�������״̬")) {
                siriusEngine.camera.reset();
            }

            ImGui::PopID();
        }
        ImGui::PopStyleColor(1);    // �������ѡ��������ɫend

        // ��������ѡ����
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.55f, 0.53f, 0.31f, 1.0f));  // ��������ѡ��������ɫbegin
        if (ImGui::CollapsingHeader(u8"��������", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID(u8"��������");

			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar(u8"������ʽ��", tab_bar_flags)) {
				int& selectIndex = siriusEngine.currentSelectedPointLightIndex;
				if (selectIndex >= 0) {
                    PointLight* currentPointLight = siriusEngine.scenePointLights[selectIndex];
					
                    // ��ǰѡ�е��Դѡ���
                    if (ImGui::BeginTabItem(u8"���Դ")) {
						ImGui::PushItemWidth(ImGui::GetWindowSize().x - 100.0f);
						std::stringstream ss;
						ss << selectIndex + 1 << u8". ���Դ";
						if (ImGui::BeginCombo("##scenePointLightsCombo", ss.str().c_str())) {
							for (unsigned int i = 0; i < siriusEngine.scenePointLights.size(); ++i) {
								bool selected = (i == siriusEngine.currentSelectedPointLightIndex);
								std::stringstream ss;
								ss << i + 1 << u8". ���Դ";
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
						if (ImGui::Button(u8"����")) {
							currentPointLight->reset();
						}

                        // ��ӵ��Դ
                        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2.0f - 120.0f, ImGui::GetCursorPos().y));
                        if (ImGui::Button(u8"��ӵ��Դ", ImVec2(100.0f, 0.0f))) {
                            PointLight* light = LoadPresets::loadPointLight();
                            siriusEngine.scenePointLights.push_back(light);
                            siriusEngine.currentSelectedPointLightIndex = siriusEngine.scenePointLights.size() - 1;
                        }

                        ImGui::SameLine();

                        // ɾ����ǰ���Դ
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.44f, 0.17f, 0.17f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.27f, 0.27f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.85f, 0.31f, 0.31f, 1.0f));
                        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2.0f + 20.0f, ImGui::GetCursorPos().y));

                        if (ImGui::Button(u8"ɾ�����Դ", ImVec2(100.0f, 0.0f))) {
                            if (selectIndex >= 0) {
                                delete siriusEngine.scenePointLights[selectIndex];
                                siriusEngine.scenePointLights.erase(siriusEngine.scenePointLights.begin() + selectIndex);
                                if (selectIndex == siriusEngine.scenePointLights.size())
                                    --selectIndex;
                            }
                        }
                        ImGui::PopStyleColor(3);

						ImGui::Separator();

						ImGui::ColorEdit3(u8"������", glm::value_ptr(currentPointLight->ambient));
						ImGui::ColorEdit3(u8"������", glm::value_ptr(currentPointLight->diffuse));
						ImGui::ColorEdit3(u8"���淴��", glm::value_ptr(currentPointLight->specular));

						ImGui::DragFloat3(u8"��˥������", glm::value_ptr(currentPointLight->attenuationFactors), IMGUI_DRAG_SPEED_SUPER_SLOW, 0.0f, 1.0f);
						ImGui::SameLine();
						HelpMarker(u8"��˥������ϵ������: �����һ���������");

						ImGui::Separator();

						ImGui::DragFloat3(u8"λ��", glm::value_ptr(currentPointLight->position), IMGUI_DRAG_SPEED_SLOW);

						ImGui::EndTabItem();
					}
                }

                if (ImGui::BeginTabItem(u8"�����")) {
                    DirLight& dirLight = siriusEngine.dirLight;

                    ImGui::Text(u8"��ʾ�����");
                    ImGui::SameLine();
                    ImGui::Checkbox("##enableDirectionalLight", &dirLight.enabled);
                    ImGui::SameLine();
                    if (ImGui::Button(u8"����")) {
                        dirLight.reset();
                    }

                    ImGui::Separator();

                    ImGui::ColorEdit3(u8"������", glm::value_ptr(dirLight.ambient));
                    ImGui::ColorEdit3(u8"������", glm::value_ptr(dirLight.diffuse));
                    ImGui::ColorEdit3(u8"���淴��", glm::value_ptr(dirLight.specular));

                    ImGui::Separator();

                    ImGui::DragFloat3(u8"����", glm::value_ptr(dirLight.direction), IMGUI_DRAG_SPEED_SLOW);
                    ImGui::SliderFloat(u8"�뾶", &dirLight.radius, DIR_RADIUS_MIN, DIR_RADIUS_MAX, "%.1f");

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::PopID();
        }
        ImGui::PopStyleColor(1);    // ��������ѡ��������ɫend
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
            // ѡ��
            ImGui::Checkbox(u8"��Ȳ���", &siriusEngine.isDepthTestOn);
            ImGui::SameLine();
            ImGui::Checkbox(u8"ģ�����", &siriusEngine.isStencilTestOn);
            ImGui::SameLine();
            ImGui::Checkbox(u8"�����޳�", &siriusEngine.isFaceCullingOn);
            const char* postProcessingNames[postProcessingCount] = {u8"����Ч", u8"����", u8"�Ҷ�", u8"��", u8"ģ��", u8"��Ե"};
            const char* postProcessingName = (siriusEngine.postProcessing >= 0 && siriusEngine.postProcessing < postProcessingCount) ?
                postProcessingNames[siriusEngine.postProcessing] : u8"δ֪";
            ImGui::SliderInt(u8"������Ч", &siriusEngine.postProcessing, 0, postProcessingCount - 1, postProcessingName);
            ImGui::Checkbox(u8"MSAA�����", &siriusEngine.isMSAAOn);
            ImGui::SameLine();
            ImGui::Checkbox(u8"Gamma����", &siriusEngine.isGammaOn);

            ImGui::Separator();
            
            ImGui::Checkbox(u8"bias�Ż�Ӳ��Ӱ", &siriusEngine.isBiasShadowOn);
            ImGui::SameLine();
            ImGui::Checkbox(u8"�����޳��Ż�Ӳ��Ӱ", &siriusEngine.isCullShadowOn);

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

