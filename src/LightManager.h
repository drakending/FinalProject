#pragma once
#include<base/light.h>
#include<memory>
namespace Game {
	class LightManager
	{
	public:
		std::shared_ptr<DirectionalLight> directionalLight;
		std::shared_ptr<PointLight> pointLight;
	};

}

