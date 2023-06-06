#pragma once
#include<iostream>
#include<map>

#include<base/glsl_program.h>
#include<base/model.h>
#include<base/texture2d.h>
#include<base/camera.h>
#include<base/light.h>
#include<storage.h>
namespace Game
{
	class scene
	{
		std::vector<std::shared_ptr<Model>> NormalModelList;
		std::map<int, int> modelTexture;
		std::vector<std::shared_ptr<Texture>> TextureList;
		std::unique_ptr<PerspectiveCamera> _camera;
		std::unique_ptr<DirectionalLight> _light;
		std::unique_ptr<GLSLProgram> modelShader;
	public:
		scene(storage& storageManger);
		void draw();

	};


}

