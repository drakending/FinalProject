#include "NormalScene.h"

Game::NormalScene::NormalScene(StorageManager* storageManager)
{
	modelShader.reset(new GLSLProgram());
	modelShader->attachVertexShaderFromFile("./src/shader/model.vs");
	modelShader->attachFragmentShaderFromFile("./src/shader/model.fs");

	lightShader.reset(new GLSLProgram());
	lightShader->attachVertexShaderFromFile("./src/shader/light.vs");
	lightShader->attachFragmentShaderFromFile("./src/shader/light.fs");
	modelShader->link();
	lightShader->link();

	_directionalDepthShader.reset(new GLSLProgram);
	_directionalDepthShader->attachVertexShaderFromFile("./src/shader/directional_depth.vert"
	);
	_directionalDepthShader->attachFragmentShaderFromFile("./src/shader/directional_depth.frag"
	);
	_directionalDepthShader->link();

	_omnidirectionalDepthShader.reset(new GLSLProgram);
	_omnidirectionalDepthShader->attachVertexShaderFromFile(
		"./src/shader/omnidirectional_depth.vert");
	_omnidirectionalDepthShader->attachFragmentShaderFromFile(
		"./src/shader/omnidirectional_depth.frag");
	_omnidirectionalDepthShader->link();

	lightCube = storageManager->getModel("cube");

	std::shared_ptr<Model> _model = storageManager->getModel("african_head");
	ModelTex t(_model);
	t.attach(ModelTex::DIFFUSEMAP, storageManager->getTexture("african_head_diffuse"));
	t.attach(ModelTex::SPECULARMAP, storageManager->getTexture("african_head_spec"));
	t.storageManger = storageManager;
	ModelTex t2(storageManager->getModel("african_head_eye_inner"));
	t2.attach(ModelTex::DIFFUSEMAP, storageManager->getTexture("african_head_eye_inner_diffuse"));
	t2.attach(ModelTex::SPECULARMAP, storageManager->getTexture("african_head_eye_inner_spec"));
	t2.storageManger = storageManager;
	ModelTex t3(storageManager->getModel("floor"));
	t3.attach(ModelTex::DIFFUSEMAP, storageManager->getTexture("floor_diffuse"));
	IntegrateModel im;
	im.subModel.push_back(t);
	im.subModel.push_back(t2);
	im.subModel.push_back(t3);
	im.name = "black people";
	IntegrateModelList.push_back(im);


	_camera.reset(new PerspectiveCamera(
		glm::radians(50.0f), 1.0f * 1920 / 1080, 0.1f, 10000.0f));
	_camera->transform.position.z = 7.0f;
	_camera->transform.position.y = 1.0f;
	_camera->transform.lookAt(glm::vec3(0));

	_light.reset(new DirectionalLight());
	_light->transform.position = glm::vec3(-10.0f, 10.0f, 10.0f);
	_light->transform.lookAt(glm::vec3(0));
	_pointLight.reset(new PointLight());
	_pointLight->transform.position = glm::vec3(3, 3, 0);
	_pointLight->transform.scale = glm::vec3(0.2);
	_pointLight->kc = 1.0f;
	_pointLight->kl = 0.09f;
	_pointLight->kq = 0.032f;



	std::vector<float> borderColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	// directional light shadow map
	// init depth texture and its corresponding framebuffer
	_depthTexture.reset(new Texture2D(
		GL_DEPTH_COMPONENT, 4096, 4096, GL_DEPTH_COMPONENT,
		GL_FLOAT));
	_depthTexture->bind();
	_depthTexture->setParamterInt(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	_depthTexture->setParamterInt(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	_depthTexture->setParamterInt(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	_depthTexture->setParamterInt(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	_depthTexture->setParamterFloatVector(GL_TEXTURE_BORDER_COLOR, borderColor);
	_depthTexture->unbind();

	_depthFbo.reset(new Framebuffer);
	_depthFbo->bind();
	_depthFbo->drawBuffer(GL_NONE);
	_depthFbo->readBuffer(GL_NONE);

	_depthFbo->attachTexture2D(*_depthTexture, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D);

	GLenum status = _depthFbo->checkStatus();
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("depthFbo illegal status: " + _depthFbo->getDiagnostic(status));
	}

	_depthFbo->unbind();

	// point light shadow map
	// init depth cube texture and its corresponding framebuffers
	_depthCubeTexture.reset(new TextureCubemap(
		GL_DEPTH_COMPONENT, 4096, 4096, GL_DEPTH_COMPONENT,
		GL_FLOAT));
	_depthCubeTexture->bind();
	_depthCubeTexture->setParamterInt(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	_depthCubeTexture->setParamterInt(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	_depthCubeTexture->setParamterInt(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	_depthCubeTexture->setParamterInt(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	_depthCubeTexture->setParamterInt(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	_depthCubeTexture->unbind();

	for (size_t i = 0; i < _depthCubeFbos.size(); ++i) {
		_depthCubeFbos[i].reset(new Framebuffer);
		_depthCubeFbos[i]->bind();
		_depthCubeFbos[i]->drawBuffer(GL_NONE);
		_depthCubeFbos[i]->readBuffer(GL_NONE);
		_depthCubeFbos[i]->attachTexture2D(
			*_depthCubeTexture, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<int>(i));

		GLenum status = _depthCubeFbos[i]->checkStatus();
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			throw std::runtime_error(
				"_depthCubeFbos illegal status: " + _depthCubeFbos[i]->getDiagnostic(status));
		}

		_depthCubeFbos[i]->unbind();
	}


}

void Game::NormalScene::draw()
{
	if (enableShadow)
	{

		updateDirectionalLightSpaceMatrix();
		updatePointLightSpaceMatrices();
		renderShadowMaps();
	}

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
	modelShader->setUniformFloat("pointLightZfar", _pointLightZfar);
	modelShader->setUniformMat4("directionalLightSpaceMatrix", _directionalLightSpaceMatrix);
	modelShader->setUniformBool("enableShadow", enableShadow);
	if (enableShadow)
	{
		modelShader->setUniformInt("directionalFilterRadius", _directionalFilterRadius);
		modelShader->setUniformBool("enableOmnidirectionalPCF", _enableOmnidirectionalPCF);
		modelShader->setUniformInt("depthTexture", 2);
		_depthTexture->bind(2);
		modelShader->setUniformInt("depthCubeTexture", 3);
		_depthCubeTexture->bind(3);
	}
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

void Game::NormalScene::updateDirectionalLightSpaceMatrix()
{
	_directionalLightSpaceMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f)
		*            // projection
		glm::lookAt( // view
			glm::vec3(0.0f) - _light->transform.getFront(),
			glm::vec3(0.0f, 0.0f, 0.0f), Transform::getDefaultUp());
}

void Game::NormalScene::updatePointLightSpaceMatrices()
{
	const glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, _pointLightZfar);
	const glm::vec3& eye = _pointLight->transform.position;
	const glm::mat4 views[6] = {
		glm::lookAt(eye, eye + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(eye, eye + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(eye, eye + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::lookAt(eye, eye + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(eye, eye + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(eye, eye + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	};
	for (size_t i = 0; i < _pointLightSpaceMatrices.size(); ++i) {
		_pointLightSpaceMatrices[i] = projection * views[i];
	}
}

void Game::NormalScene::renderShadowMaps()
{
	renderPointLightShadowMap();
	renderDirectionalLightShadowMap();

}

void Game::NormalScene::renderDirectionalLightShadowMap()
{
	glViewport(0, 0, 4096, 4096);

	_directionalDepthShader->use();
	_directionalDepthShader->setUniformMat4("lightSpaceMatrix", _directionalLightSpaceMatrix);

	_depthFbo->bind();
	glClear(GL_DEPTH_BUFFER_BIT);

	renderSceneFromLight(*_directionalDepthShader);
	_depthFbo->unbind();

	glViewport(0, 0, 1920, 1080);
}

void Game::NormalScene::renderSceneFromLight(const GLSLProgram& shader)
{
	for (int i = 0; i < IntegrateModelList.size(); i++)
	{
		shader.setUniformMat4("model", IntegrateModelList[i].transform.getLocalMatrix());
		IntegrateModelList[i].draw();
	}
}

void Game::NormalScene::renderPointLightShadowMap()
{
	glViewport(0, 0, 4096, 4096);

	_omnidirectionalDepthShader->use();
	_omnidirectionalDepthShader->setUniformFloat("zFar", _pointLightZfar);
	_omnidirectionalDepthShader->setUniformVec3("lightPosition", _pointLight->transform.position);

	// render the scene 6 times for each cubemap face
	// _pointLightSpaceMatrices must be updated before rendering
	for (size_t i = 0; i < _depthCubeFbos.size(); ++i) {
		_depthCubeFbos[i]->bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		_omnidirectionalDepthShader->setUniformMat4(
			"lightSpaceMatrix", _pointLightSpaceMatrices[i]);
		renderSceneFromLight(*_omnidirectionalDepthShader);

		_depthCubeFbos[i]->unbind();
	}

	glViewport(0, 0, 1920, 1080);
}
