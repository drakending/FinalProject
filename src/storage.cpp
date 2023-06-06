﻿#include "storage.h"
#include <filesystem>
Game::storage::storage()
{
	std::filesystem::path path(modelRootDir);
	if (!std::filesystem::exists(path))
	{
		std::cout << "no such directory" << std::endl;
		return;
	}
	std::filesystem::directory_entry entry(path);		//文件入口
	if (entry.status().type() != std::filesystem::file_type::directory)	//这里用了C++11的强枚举类型
	{
		std::cout << "no such directory" << std::endl;
	}
	std::filesystem::directory_iterator list(path);	        //文件入口容器
	for (auto& it : list)
	{
		std::string filename = it.path().filename().string();
		modelList.push_back(filename.substr(0, filename.rfind(".")));
	}
	for (auto& it : modelList)
	{
		loadModel(it);
	}
	std::filesystem::path path2(textureRootDir);
	if (!std::filesystem::exists(path2))
	{
		std::cout << "no such directory" << std::endl;
		return;
	}
	std::filesystem::directory_entry entry2(path2);		//文件入口
	if (entry2.status().type() != std::filesystem::file_type::directory)	//这里用了C++11的强枚举类型
	{
		std::cout << "no such directory" << std::endl;
	}
	std::filesystem::directory_iterator list2(path2);	        //文件入口容器
	for (auto& it : list2)
	{
		std::string filename = it.path().filename().string();
		textureList.push_back(filename);
	}
	for (auto& it : textureList)
	{
		loadTexture(it);
	}
}

Game::storage::~storage()
{

}
bool Game::storage::loadModel(std::string name)
{
	std::shared_ptr<Model> _model;
	_model.reset(new Model(modelRootDir + name + modelPostFix));
	modelStorage[name] = _model;
	return true;
}

std::shared_ptr<Model> Game::storage::getModel(std::string name)
{
	return modelStorage[name];
}

bool Game::storage::loadTexture(std::string name)
{
	std::shared_ptr<Texture2D> texture = std::make_shared<ImageTexture2D>(textureRootDir + name);
	textureStorage[name.substr(0, name.rfind('.'))] = texture;
	return true;
}

std::shared_ptr<Texture2D> Game::storage::getTexture(std::string name)
{
	return textureStorage[name];
}


