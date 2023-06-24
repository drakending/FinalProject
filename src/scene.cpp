#include "scene.h"

Game::scene::scene(StorageManager* storageManager)
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
	std::shared_ptr<Model> _model = storageManager->getModel("african_head");
	ModelTex t(_model);
	t.attach(ModelTex::DIFFUSEMAP, storageManager->getTexture("african_head_diffuse"));
	t.attach(ModelTex::SPECULARMAP, storageManager->getTexture("african_head_spec"));
	t.storageManger = storageManager;
	ModelTex t2(storageManager->getModel("african_head_eye_inner"));
	t2.attach(ModelTex::DIFFUSEMAP, storageManager->getTexture("african_head_eye_inner_diffuse"));
	t2.attach(ModelTex::SPECULARMAP, storageManager->getTexture("african_head_eye_inner_spec"));
	t2.storageManger = storageManager;
	IntegrateModel im;
	im.subModel.push_back(t);
	im.subModel.push_back(t2);
	im.name = "black people";
	IntegrateModelList.push_back(im);


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

void Game::scene::draw()
{
	modelShader->use();
	modelShader->setUniformVec3("viewPos", _camera->transform.position);
	modelShader->setUniformVec3("dirLight.direction", _light->transform.getFront());
	modelShader->setUniformVec3("dirLight.ambient", _light->color);
	modelShader->setUniformVec3("dirLight.diffuse", _light->color);
	modelShader->setUniformVec3("dirLight.specular", _light->color);
	modelShader->setUniformVec3("pointLight.position", _pointLight->transform.position);
	modelShader->setUniformFloat("pointLight.constant", _pointLight->kc);
	modelShader->setUniformFloat("pointLight.linear", _pointLight->kl);
	modelShader->setUniformFloat("pointLight.quadratic", _pointLight->kq);
	modelShader->setUniformVec3("pointLight.ambient", _pointLight->color);
	modelShader->setUniformVec3("pointLight.diffuse", _pointLight->color);
	modelShader->setUniformVec3("pointLight.specular", _pointLight->color);
	for (int i = 0; i < IntegrateModelList.size(); i++)
	{
		auto model = IntegrateModelList[i];
		modelShader->setUniformInt("texture_diffuse1", 0);
		modelShader->setUniformInt("texture_specular1", 1);
		modelShader->setUniformMat4("model", model.transform.getLocalMatrix());
		modelShader->setUniformMat4("view", _camera->getViewMatrix());
		modelShader->setUniformMat4("projection", _camera->getProjectionMatrix());
		model.draw();
	}
	lightShader->use();
	lightShader->setUniformMat4("model", _pointLight->transform.getLocalMatrix());
	lightShader->setUniformMat4("view", _camera->getViewMatrix());
	lightShader->setUniformMat4("projection", _camera->getProjectionMatrix());
	lightShader->setUniformVec3("lightColor", _pointLight->color);
	lightCube->draw();
}
