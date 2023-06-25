#pragma once
#include<base/light.h>
#include<memory>
namespace Game {
	class LightManager
	{
	public:
		DirectionalLight* directionalLight;
		PointLight* pointLight;
	};

}

