#pragma once
#include<iostream>
#include<map>

#include<base/glsl_program.h>
#include<base/model.h>
#include<base/texture2d.h>
#include<base/camera.h>
#include<base/light.h>
#include<StorageManager.h>
#include<IntegrateModel.h>
namespace Game
{
	class scene
	{
	public:
		std::vector<IntegrateModel> IntegrateModelList;
		std::shared_ptr<PerspectiveCamera> _camera;
		std::shared_ptr<DirectionalLight> _light;
		std::shared_ptr<PointLight> _pointLight;
		std::unique_ptr<GLSLProgram> modelShader;
		std::unique_ptr<GLSLProgram> lightShader;
		std::shared_ptr<Model> lightCube;
		scene(StorageManager* storageManger);
		void draw();

	};


}

