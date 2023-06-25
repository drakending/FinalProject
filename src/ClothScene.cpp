#include "ClothScene.h"

Game::ClothScene::ClothScene(StorageManager* storageManager) :cloth(glm::vec3(-3.2, 2.4, -8.6), 6, 6)
{
	this->storageManager = storageManager;
	initRenderResource();
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
	running = true;
}

void Game::ClothScene::draw()
{
	glm::vec3 gravity(0.0, -9.8 / cloth.iterationFreq, 0.0);

	clothShader->use();
	clothShader->setUniformVec3("viewPos", _camera->transform.position);
	clothShader->setUniformVec3("dirLight.direction", _light->transform.getFront());
	clothShader->setUniformVec3("dirLight.ambient", _light->color);
	clothShader->setUniformVec3("dirLight.diffuse", _light->color);
	clothShader->setUniformVec3("dirLight.specular", _light->color);
	clothShader->setUniformVec3("pointLight.position", _pointLight->transform.position);
	clothShader->setUniformFloat("pointLight.constant", _pointLight->kc);
	clothShader->setUniformFloat("pointLight.linear", _pointLight->kl);
	clothShader->setUniformFloat("pointLight.quadratic", _pointLight->kq);
	clothShader->setUniformVec3("pointLight.ambient", _pointLight->color);
	clothShader->setUniformVec3("pointLight.diffuse", _pointLight->color);
	clothShader->setUniformVec3("pointLight.specular", _pointLight->color);

	clothShader->setUniformMat4("model", cloth.transform.getLocalMatrix());
	clothShader->setUniformMat4("view", _camera->getViewMatrix());
	clothShader->setUniformMat4("projection", _camera->getProjectionMatrix());
	clothShader->setUniformInt("tex", 0);
	if (running)
	{
		for (int i = 0; i < cloth.iterationFreq; i++)
		{
			cloth.computeForce(0.01, gravity);
			cloth.update(0.01);
		}
		cloth.computeNormal();
	}
	// Update all the positions of nodes
	auto nodeCount = cloth.faces.size();
	for (int i = 0; i < cloth.faces.size(); i++) { // Tex coordinate dose not change
		auto n = cloth.faces[i];
		pos[i] = n->position;
		normal[i] = n->normal;
	}
	clothShader->use();
	glBindVertexArray(clothVao);

	glBindBuffer(GL_ARRAY_BUFFER, clothAttribVbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nodeCount * sizeof(glm::vec3), pos.data());
	glBindBuffer(GL_ARRAY_BUFFER, clothAttribVbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nodeCount * sizeof(glm::vec3), normal.data());
	glBindBuffer(GL_ARRAY_BUFFER, clothAttribVbo[2]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nodeCount * sizeof(glm::vec2), texCoord.data());


	clothTexture->bind(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, nodeCount);
	// End flushing
	glDisable(GL_BLEND);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Game::ClothScene::initRenderResource()
{

	for (int i = 0; i < cloth.faces.size(); i++)
	{
		auto t = cloth.faces[i];
		pos.push_back(t->position);
		normal.push_back(t->normal);
		texCoord.push_back(t->texCoord);
	}

	auto size = cloth.faces.size();
	glGenVertexArrays(1, &clothVao);
	glGenBuffers(3, clothAttribVbo);
	glBindVertexArray(clothVao);
	glBindBuffer(GL_ARRAY_BUFFER, clothAttribVbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec3), pos.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, clothAttribVbo[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec3), normal.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, clothAttribVbo[2]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec2), texCoord.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	clothTexture = storageManager->getTexture("wood");
	clothShader.reset(new GLSLProgram());
	clothShader->attachFragmentShaderFromFile("./src/shader/cloth.fs");
	clothShader->attachVertexShaderFromFile("./src/shader/cloth.vs");
	clothShader->link();
}
