#include <iostream>
#include "SDL.h"
#include <cstdint>
#include <chrono>
#include "sdlWrapper.hpp"
#include "hailEngine/hailEngine.hpp"
std::chrono::high_resolution_clock::time_point startFrame, endFrame;
std::chrono::duration<double> deltaTime;

int main() {
	double y = 0;
	std::cout << "Hello World!" << std::endl;
	SDLwrapper::Window * window = new SDLwrapper::Window(640, 480, "Project Platformer");
	startFrame = std::chrono::high_resolution_clock::now();
	while (true) {
		endFrame = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::duration<double> >(endFrame - startFrame);
		startFrame = endFrame;
		y += deltaTime.count();
		window->clearScreen(new SDLwrapper::Color(0, 0, 0, 255));
		window->runInput();
		window->drawRect(new SDLwrapper::Color(0, 0, 255, 255), 10, y, 100, 100);
		window->presentWindow();
	}
	return 0;
}