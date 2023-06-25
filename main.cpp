#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<base.h>
#include<iostream>
#include<base/glsl_program.h>
#include<base/model.h>
#include<base/texture2d.h>
#include<base/camera.h>
#include<base/light.h>
#include<base/input.h>
#include<scene.h>
#include<StorageManager.h>
#include<CameraManager.h>
#include<LightManager.h>
#include<ClothManager.h>
#include<NormalScene.h>
#include<BasicScene.h>
#include<ClothScene.h>
#include<gui.h>
using namespace Game;
Input input;

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	input.mouse.move.xNow = static_cast<float>(xPos);
	input.mouse.move.yNow = static_cast<float>(yPos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT: input.mouse.press.left = true; break;
		case GLFW_MOUSE_BUTTON_MIDDLE: input.mouse.press.middle = true; break;
		case GLFW_MOUSE_BUTTON_RIGHT: input.mouse.press.right = true; break;
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT: input.mouse.press.left = false; break;
		case GLFW_MOUSE_BUTTON_MIDDLE: input.mouse.press.middle = false; break;
		case GLFW_MOUSE_BUTTON_RIGHT: input.mouse.press.right = false; break;
		}
	}
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	input.mouse.scroll.xOffset = static_cast<float>(xOffset);
	input.mouse.scroll.yOffset = static_cast<float>(yOffset);
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key != GLFW_KEY_UNKNOWN) {
		input.keyboard.keyStates[key] = action;
	}
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = glfwCreateWindow(Base::WindowWidth, Base::WindowHeight, Base::Title.c_str(), NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	StorageManager storageManger;
	CameraManager cameraManager;
	LightManager lightManager;
	ClothManager clothManager;

	NormalScene defaultScene(&storageManger);
	defaultScene.sceneName = "default scene";
	cameraManager.registerCamera(defaultScene._camera);
	lightManager.directionalLight = defaultScene._light.get();
	lightManager.pointLight = defaultScene._pointLight.get();

	BasicScene basicScene(&storageManger);
	basicScene.sceneName = "basic scene";
	ClothScene clothScene(&storageManger);
	clothScene.sceneName = "cloth scene";


	gui defaultGui(&defaultScene);
	defaultGui.initEnv(window);
	defaultGui.cameraManager = &cameraManager;
	defaultGui.lightManager = &lightManager;
	defaultGui.clothManager = &clothManager;
	defaultGui.sceneList.push_back(&defaultScene);
	defaultGui.sceneList.push_back(&basicScene);
	defaultGui.sceneList.push_back(&clothScene);
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		cameraManager.handleInput(input);
		clothManager.handleInput(input);
		defaultGui.scene_->draw();
		defaultGui.newFrame();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;

}



