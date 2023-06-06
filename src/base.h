#pragma once

#include <string>
namespace Game {
	class Base {
	public:
		static unsigned int WindowWidth, WindowHeight;
		static const std::string Title;
	};
	unsigned int Base::WindowWidth = 1920;
	unsigned int Base::WindowHeight = 1080;
	const std::string Base::Title = "Final Project";
}
