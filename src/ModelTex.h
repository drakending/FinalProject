#pragma once
#include<map>
#include<memory>
#include<vector>
#include<base/texture.h>
#include<base/model.h>
#include"StorageManager.h"
class ModelTex
{
public:
	enum TextureType {
		DIFFUSEMAP, SPECULARMAP
	};
	std::map<TextureType, std::shared_ptr<Texture>> TextureTypeMap;
	std::map<TextureType, char*> TextureNameMap;
	std::shared_ptr<Model> _model;
	ModelTex(std::shared_ptr<Model> model) :_model(model) {};
	Game::StorageManager* storageManger;
	void attach(TextureType type, std::shared_ptr<Texture> tex) {
		TextureTypeMap[type] = tex;
		TextureNameMap[type] = new char[100];
		strcpy(TextureNameMap[type], tex.get()->name.c_str());
	}
	void changeTex()
	{
		for (auto& pair : TextureNameMap)
		{
			TextureType type = pair.first;
			if (strcmp(TextureTypeMap[type].get()->name.c_str(), TextureNameMap[type]) != 0)
			{
				auto tmp = storageManger->getTexture(TextureNameMap[type]);
				if (tmp != nullptr)
					TextureTypeMap[type] = tmp;

			}
		}
	}
	void draw()
	{
		for (auto& it : TextureTypeMap)
		{
			if (it.first == DIFFUSEMAP)				TextureTypeMap[DIFFUSEMAP]->bind(0);
			else if (it.first == SPECULARMAP)
				TextureTypeMap[SPECULARMAP]->bind(1);
		}
		_model->draw();
		for (auto& it : TextureTypeMap)
		{
			if (it.first == DIFFUSEMAP)
				TextureTypeMap[DIFFUSEMAP]->unbind();
			else if (it.first == SPECULARMAP)
				TextureTypeMap[SPECULARMAP]->unbind();
		}
	}
};

