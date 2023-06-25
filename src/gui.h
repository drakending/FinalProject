#pragma once
#include<imgui/imgui.h>
#include<imgui/imgui_impl_glfw.h>
#include<imgui/imgui_impl_opengl3.h>
#include<scene.h>
#include<NormalScene.h>
#include<BasicScene.h>
#include<ClothScene.h>
#include<CameraManager.h>
#include<LightManager.h>
#include<ClothManager.h>
class gui
{

public:
	std::vector<Game::scene*> sceneList;
	Game::scene* scene_;
	Game::CameraManager* cameraManager;
	Game::LightManager* lightManager;
	Game::ClothManager* clothManager;

	gui(Game::scene* scene);
	void cameraPanel();
	void lightPanel();
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

