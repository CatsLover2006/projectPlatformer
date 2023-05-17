#include <iostream>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <vector>
#include <random>
#include "sdlWrapper.hpp"
std::chrono::high_resolution_clock::time_point startFrame, endFrame;
std::chrono::duration<double> deltaTime;

int main() {
	SDLwrapper::Window * window = new SDLwrapper::Window(640, 480, "Project Platformer");
	SDLwrapper::Image * testCat = new SDLwrapper::Image("assets/textures/Cat.png", window);
	window->clearScreen(new SDLwrapper::Color(10, 200, 255, 255));
	window->runInput();
	window->presentWindow();
	window->runInput();
	window->presentWindow();
	while (true) {
		endFrame = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::duration<double> >(endFrame - startFrame);
		startFrame = endFrame;
		std::cout << deltaTime.count() << "s" << std::endl;
		window->clearScreen(new SDLwrapper::Color(10, 200, 255, 255));
		window->runInput();
		window->drawImage(testCat, 10, 10, 250, 200);
		window->presentWindow();
	}
	window->quit();
	return 0;
}