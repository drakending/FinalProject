#pragma once
#include<base/input.h>
#include<cloth/Cloth.h>
namespace Game {
	class ClothManager
	{
	public:
		bool managerEnable;
		Cloth* cloth;
		void registerCloth(Cloth* cloth);
		float windForce = 15;
		void handleInput(Input& input);
	};

}

