#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<base.h>
#include<iostream>
#include<base/glsl_program.h>
#include<base/model.h>
#include<base/texture2d.h>
#include<base/camera.h>
#include<base/light.h>
#include<scene.h>
#include<storage.h>
using namespace Game;
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
	storage storageManger;
	scene defaultScene(storageManger);
	glm::vec3 selected_color = glm::vec3(0.0);
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		defaultScene.draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;

}