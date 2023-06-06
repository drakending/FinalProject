#include "scene.h"

Game::scene::scene(storage& storageManager)
{
	modelShader.reset(new GLSLProgram());
	modelShader->attachVertexShaderFromFile("./src/shader/model.vs");
	modelShader->attachFragmentShaderFromFile("./src/shader/model.fs");
	modelShader->link();
	std::shared_ptr<Model> _model = storageManager.getModel("african_head");
	ModelTex t(_model);
	t.attach(ModelTex::DIFFUSEMAP, storageManager.getTexture("african_head_diffuse"));
	t.attach(ModelTex::SPECULARMAP, storageManager.getTexture("african_head_spec"));
	//ModelTex t2(storageManager.getModel("african_head_eye_outer"));
	//t2.attach(ModelTex::DIFFUSEMAP, storageManager.getTexture("african_head_eye_outer_diffuse"));
	//t2.attach(ModelTex::SPECULARMAP, storageManager.getTexture("african_head_eye_outer_spec"));
	ModelTex t3(storageManager.getModel("african_head_eye_inner"));
	t3.attach(ModelTex::DIFFUSEMAP, storageManager.getTexture("african_head_eye_inner_diffuse"));
	t3.attach(ModelTex::SPECULARMAP, storageManager.getTexture("african_head_eye_inner_spec"));


	IntegrateModel im;
	im.subModel.push_back(t);
	im.subModel.push_back(t3);
	IntegrateModelList.push_back(im);

	_camera.reset(new PerspectiveCamera(
		glm::radians(50.0f), 1.0f * 1920 / 1080, 0.1f, 10000.0f));
	_camera->transform.position.z = 10.0f;
	_light.reset(new DirectionalLight());
	_light->transform.rotation =
		glm::angleAxis(glm::radians(45.0f), glm::normalize(glm::vec3(-1.0f, -2.0f, -1.0f)));
}

void Game::scene::draw()
{
	glm::vec3 selected_color(0.0);
	modelShader->use();
	modelShader->setUniformVec3("viewPos", _camera->transform.position);
	modelShader->setUniformVec3("selected_color", selected_color);
	modelShader->setUniformVec3("dirLight.direction", _light->transform.getFront());
	modelShader->setUniformVec3("dirLight.ambient", _light->color);
	modelShader->setUniformVec3("dirLight.diffuse", _light->color);
	modelShader->setUniformVec3("dirLight.specular", _light->color);

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
}
