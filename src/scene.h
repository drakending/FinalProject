#pragma once
#include<string>
namespace Game {
	class scene
	{
	public:
		scene() = default;
		~scene() = default;
		virtual void draw() = 0;
		std::string sceneName;
	};

}

