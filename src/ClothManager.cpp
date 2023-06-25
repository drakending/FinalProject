#include "ClothManager.h"

void Game::ClothManager::registerCloth(Cloth* cloth)
{
	this->cloth = cloth;
	managerEnable = true;
}

void Game::ClothManager::handleInput(Input& input)
{
	if (!managerEnable) return;
	if (input.keyboard.keyStates[GLFW_KEY_UP] != GLFW_RELEASE) {
		cloth->addForce(glm::vec3(0, 0, -windForce));
	}
	if (input.keyboard.keyStates[GLFW_KEY_DOWN] != GLFW_RELEASE) {
		cloth->addForce(glm::vec3(0, 0, windForce));
	}
	if (input.keyboard.keyStates[GLFW_KEY_LEFT] != GLFW_RELEASE) {
		cloth->addForce(glm::vec3(-windForce, 0, 0));
	}
	if (input.keyboard.keyStates[GLFW_KEY_RIGHT] != GLFW_RELEASE) {
		cloth->addForce(glm::vec3(windForce, 0, 0));
	}
}
