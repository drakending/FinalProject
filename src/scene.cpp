#include "scene.h"

Game::scene::scene(storage& storageManager)
{
	modelShader.reset(new GLSLProgram());
	modelShader->attachVertexShaderFromFile("./src/shader/model.vs");
	modelShader->attachFragmentShaderFromFile("./src/shader/model.fs");
	modelShader->link();

	std::shared_ptr<Texture2D> texture = storageManager.getTexture("texture");
	TextureList.push_back(texture);

	std::shared_ptr<Model> _model = storageManager.getModel("african_head");
	NormalModelList.push_back(_model);

	_camera.reset(new PerspectiveCamera(
		glm::radians(50.0f), 1.0f * 1920 / 1080, 0.1f, 10000.0f));
	_camera->transform.position.z = 10.0f;
	_light.reset(new DirectionalLight());
	_light->transform.rotation =
		glm::angleAxis(glm::radians(45.0f), glm::normalize(glm::vec3(-1.0f, -2.0f, -1.0f)));
	modelTexture[0] = 0;
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

	for (int i = 0; i < NormalModelList.size(); i++)
	{
		auto _model = NormalModelList[i];
		TextureList[modelTexture[i]]->bind();
		modelShader->setUniformInt("texture_diffuse1", 0);
		modelShader->setUniformMat4("model", _model->transform.getLocalMatrix());
		modelShader->setUniformMat4("view", _camera->getViewMatrix());
		modelShader->setUniformMat4("projection", _camera->getProjectionMatrix());
		_model->draw();

	}
}
