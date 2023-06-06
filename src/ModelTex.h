#pragma once
#include<map>
#include<memory>
#include<vector>
#include<base/texture.h>
#include<base/model.h>
class ModelTex
{
public:
	enum TextureType {
		DIFFUSEMAP, SPECULARMAP
	};
	std::map<TextureType, std::shared_ptr<Texture>> TextureTypeMap;
	std::shared_ptr<Model> _model;
	ModelTex(std::shared_ptr<Model> model) :_model(model) {};
	void attach(TextureType type, std::shared_ptr<Texture> tex) {
		TextureTypeMap[type] = tex;
	}
	void draw()
	{
		for (auto& it : TextureTypeMap)
		{
			if (it.first == DIFFUSEMAP)
				TextureTypeMap[DIFFUSEMAP]->bind(0);
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

