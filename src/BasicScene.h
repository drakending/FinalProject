#pragma once
#include"scene.h"
#include"base/model.h"
#include<base/glsl_program.h>
#include<base/model.h>
#include<base/texture2d.h>
#include<base/camera.h>
#include<base/light.h>
#include"StorageManager.h"
namespace Game {
	class BasicScene :public scene
	{
	public:
		std::map< std::string, Model*  > modelList;
		std::vector<std::string> modelName;
		Model* currentModel;
		std::map<std::string, float> parameter;
		std::shared_ptr<PerspectiveCamera> _camera;
		std::shared_ptr<DirectionalLight> _light;
		std::shared_ptr<PointLight> _pointLight;
		std::unique_ptr<GLSLProgram> modelShader;
		std::unique_ptr<GLSLProgram> lightShader;
		std::shared_ptr<Model> lightCube;
		glm::vec3 diffuseTex, ambientTex, specularTex;
		Transform transform;
		BasicScene(StorageManager* storageManager);
		void draw()override;
		void loadModel(const char* name, StorageManager* storageManager);
	};
}

