#include <iostream>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include "sdlWrapper.hpp"
#include "collider.hpp"
#include "object.hpp"

#define PHYSICS_TIMESTEP 0.005

std::chrono::high_resolution_clock::time_point startFrame, endFrame;
std::chrono::duration<double> deltaTime;

double w = 200;
double h = 200;
double deltaTimer = 0;
double angle = 0;
const double pi = std::atan(1.0)*4;
int loops;

double degToRad(double deg) {return deg*pi/180;}

int main() {
	std::cout << "Physics timestep is " << PHYSICS_TIMESTEP << "s" << std::endl;
	SDLwrapper::Window * window = new SDLwrapper::Window(640, 480, "Project Platformer");
	std::cout << "Loading Game Data..." << std::endl;
	SDLwrapper::Image * testCat = new SDLwrapper::Image("assets/textures/Cat.png", window);
	std::cout << "Loaded Game Data!" << std::endl;
	window->clearScreen(new SDLwrapper::Color(10, 200, 255, 255));
	window->runInput();
	window->presentWindow();
	window->runInput();
	window->presentWindow();
	startFrame = std::chrono::high_resolution_clock::now();
	while (true) {
		endFrame = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::duration<double> >(endFrame - startFrame);
		startFrame = endFrame;
		std::cout << deltaTime.count() << "s" << std::endl;
		deltaTimer += deltaTime.count();
		loops = 0;
		window->runInput();
		std::cout << window->keysDown.size() << " keys pressed" << std::endl;
		while (deltaTimer > PHYSICS_TIMESTEP) {
			deltaTimer -= PHYSICS_TIMESTEP;
			loops++;
			if (window->keyPressed("Down") || window->keyPressed("S")) h += 100 * PHYSICS_TIMESTEP;
			if (window->keyPressed("Up") || window->keyPressed("W")) h -= 100 * PHYSICS_TIMESTEP;
			if (window->keyPressed("Right") || window->keyPressed("D")) w += 100 * PHYSICS_TIMESTEP;
			if (window->keyPressed("Left") || window->keyPressed("A")) w -= 100 * PHYSICS_TIMESTEP;
			if (window->keyPressed("C")) angle += 180 * PHYSICS_TIMESTEP;
			if (window->keyPressed("V")) angle -= 180 * PHYSICS_TIMESTEP;
		}
		std::cout << loops << " physics iterations this frame" << std::endl;
		window->clearScreen(new SDLwrapper::Color(10, 200, 255, 255));
		window->drawImageEx(testCat, 10, 10, w, h, window->keyPressed("N"), window->keyPressed("M"), angle);
		window->presentWindow();
	}
	window->quit();
	return 0;
}