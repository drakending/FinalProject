#include "BasicScene.h"

Game::BasicScene::BasicScene(StorageManager* storageManager)
{
	modelShader.reset(new GLSLProgram());
	modelShader->attachVertexShaderFromFile("./src/shader/model.vs");
	modelShader->attachFragmentShaderFromFile("./src/shader/model.fs");
	lightShader.reset(new GLSLProgram());
	lightShader->attachVertexShaderFromFile("./src/shader/light.vs");
	lightShader->attachFragmentShaderFromFile("./src/shader/light.fs");
	lightCube = storageManager->getModel("cube");
	modelShader->link();
	lightShader->link();
	ambientTex = glm::vec3(0.1);
	diffuseTex = glm::vec3(0.2);
	specularTex = glm::vec3(0.2);
	loadModel("cone", storageManager);
	loadModel("sphere", storageManager);
	loadModel("cube", storageManager);
	loadModel("cylinder", storageManager);
	loadModel("prism", storageManager);
	currentModel = modelList["cone"];

	parameter["cone_radius"] = 1.0;
	parameter["cone_height"] = 1.0;
	parameter["radius"] = 1.0;
	parameter["length"] = 1.0;
	parameter["width"] = 1.0;
	parameter["height"] = 1.0;
	parameter["cylinder_height"] = 1.0;
	parameter["cylinder_radius"] = 1.0;
	parameter["prism_height"] = 1.0;
	parameter["prism_radius"] = 1.0;
	_camera.reset(new PerspectiveCamera(
		glm::radians(50.0f), 1.0f * 1920 / 1080, 0.1f, 10000.0f));
	_camera->transform.position.z = 10.0f;

	_light.reset(new DirectionalLight());
	_light->transform.rotation =
		glm::angleAxis(glm::radians(45.0f), glm::normalize(glm::vec3(-1.0f, -2.0f, -1.0f)));
	_pointLight.reset(new PointLight());
	_pointLight->transform.position = glm::vec3(3, 3, 0);
	_pointLight->transform.scale = glm::vec3(0.2);
	_pointLight->kc = 1.0f;
	_pointLight->kl = 0.09f;
	_pointLight->kq = 0.032f;
}

void Game::BasicScene::draw()
{
	modelShader->use();
	modelShader->setUniformVec3("viewPos", _camera->transform.position);
	modelShader->setUniformVec3("dirLight.direction", _light->transform.getFront());
	modelShader->setUniformVec3("dirLight.ambient", _light->color);
	modelShader->setUniformVec3("dirLight.diffuse", _light->color);
	modelShader->setUniformVec3("dirLight.specular", _light->color);
	modelShader->setUniformVec3("pointLight.position", _pointLight->transform.position);

	modelShader->setUniformVec3("diffuseTex", diffuseTex);
	modelShader->setUniformVec3("ambientTex", ambientTex);
	modelShader->setUniformVec3("specularTex", specularTex);
	modelShader->setUniformBool("normalTexture", 1);

	modelShader->setUniformFloat("pointLight.constant", _pointLight->kc);
	modelShader->setUniformFloat("pointLight.linear", _pointLight->kl);
	modelShader->setUniformFloat("pointLight.quadratic", _pointLight->kq);
	modelShader->setUniformVec3("pointLight.ambient", _pointLight->color);
	modelShader->setUniformVec3("pointLight.diffuse", _pointLight->color);
	modelShader->setUniformVec3("pointLight.specular", _pointLight->color);
	modelShader->setUniformMat4("model", transform.getLocalMatrix());
	modelShader->setUniformMat4("view", _camera->getViewMatrix());
	modelShader->setUniformMat4("projection", _camera->getProjectionMatrix());
	currentModel->draw();
	lightShader->use();
	lightShader->setUniformMat4("model", _pointLight->transform.getLocalMatrix());
	lightShader->setUniformMat4("view", _camera->getViewMatrix());
	lightShader->setUniformMat4("projection", _camera->getProjectionMatrix());
	lightShader->setUniformVec3("lightColor", _pointLight->color);
	lightCube->draw();
}

void Game::BasicScene::loadModel(const char* name, StorageManager* storageManager)
{
	std::shared_ptr<Model> cone = storageManager->getModel(name);
	modelList[name] = cone.get();
	modelName.push_back(name);
}
