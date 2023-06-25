#pragma once
#include<scene.h>
#include<base/model.h>
#include<StorageManager.h>
#include<cloth/Cloth.h>
#include"base/skybox.h"	//gai
namespace Game {
	class ClothScene :public scene
	{
	public:
		ClothScene(StorageManager* storageManager);
		Cloth cloth;
		StorageManager* storageManager;
		std::unique_ptr<SkyBox> _skybox;	//gai
		std::shared_ptr<Texture2D> clothTexture;
		std::unique_ptr<GLSLProgram> clothShader;
		std::shared_ptr<PerspectiveCamera> _camera;
		std::shared_ptr<DirectionalLight> _light;
		std::shared_ptr<PointLight> _pointLight;

		bool running;
		void draw()override;
		void initRenderResource();
	private:
		GLuint clothVao;
		GLuint clothAttribVbo[3];
		std::vector<glm::vec3> pos;
		std::vector<glm::vec3> normal;
		std::vector<glm::vec2> texCoord;
	};

}

