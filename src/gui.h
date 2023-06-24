#pragma once
#include<imgui/imgui.h>
#include<imgui/imgui_impl_glfw.h>
#include<imgui/imgui_impl_opengl3.h>
#include<scene.h>
#include<CameraManager.h>
#include<LightManager.h>
class gui
{
	Game::scene* scene_;

public:
	Game::CameraManager* cameraManager;
	Game::LightManager* lightManager;
	gui(Game::scene* scene);
	void initEnv(GLFWwindow* window) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();
	}
	void newFrame();

};

