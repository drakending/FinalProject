#pragma once
#include<vector>
#include<map>
#include<iostream>
#include<base/model.h>
#include<base/texture.h>
#include<base/transform.h>
#include<ModelTex.h>
class IntegrateModel
{
public:
	std::vector<ModelTex> subModel;
	Transform transform;
	std::string name;

	void attach(ModelTex& t)
	{
		subModel.push_back(t);
	}
	void draw()
	{
		for (auto& t : subModel)
		{
			t.draw();
		}
	}
};

