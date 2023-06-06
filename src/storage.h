#pragma once
#include <string>
#include <memory>
#include <vector>
#include <map>
#include<base/glsl_program.h>
#include<base/model.h>
#include<base/texture2d.h>
#include<base/camera.h>
#include<base/light.h>
namespace Game {
	class storage
	{
	public:
		storage();
		~storage();
		bool loadModel(std::string name);
		std::shared_ptr<Model> getModel(std::string name);
		bool loadTexture(std::string name);
		std::shared_ptr<Texture2D> getTexture(std::string name);
		std::vector<std::string> modelList;
		std::vector<std::string> textureList;
	private:
		std::map < std::string, std::shared_ptr<Model>> modelStorage;
		std::map < std::string, std::shared_ptr<Texture2D>> textureStorage;
		const std::string modelRootDir = "./media/model/";
		const std::string modelPostFix = ".obj";
		const std::string textureRootDir = "./media/texture/";
		const std::string texturePostFix = ".png";
	};

}
