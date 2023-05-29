#include <iostream>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include "hailLib/sdlWrapper.hpp"
#include "hailLib/basemath.hpp"
#include "collider.hpp"
#include "object.hpp"
#include "playerInteractableObjects.hpp"
#include "crib.hpp"
#include "player.hpp"
#include "loadLevel.hpp"

#define PHYSICS_TIMESTEP 0.005

std::chrono::high_resolution_clock::time_point startFrame, endFrame;
std::chrono::duration<double> deltaTime;

double deltaTimer = 0;
double angle = 0;
int loops;

std::vector<ObjectHandler::Object *> level;
std::vector<ObjectHandler::DynamicObject *> enemies;
GameObjects::Player * player;
uint8_t inputState;
std::vector<SDLwrapper::Image *> playerImages;

std::vector<SDLwrapper::Image *> levelImages;
SDLwrapper::Image * debugImage;

int main() {
	std::cout << "Physics timestep is " << PHYSICS_TIMESTEP << "s" << std::endl;
	SDLwrapper::Window * window = new SDLwrapper::Window(864, 480, "Project Platformer");
	std::cout << "Loading Game Data..." << std::endl;
	debugImage = new SDLwrapper::Image("assets/textures/debug.gif", window);
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Body.gif", window));
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Head_Main.gif", window));
	playerImages.push_back(debugImage);
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Arm_Top.gif", window));
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Arm_Bottom.gif", window));
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Leg_Top.gif", window));
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Leg_Bottom.gif", window));
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Arm_Bottom_Carry.gif", window));
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Leg_Bottom_Step.gif", window));
	player = new GameObjects::Player(200, -10, playerImages);
	std::cout << "Loaded Game Data!" << std::endl;
	std::cout << "Creating Objects..." << std::endl;
	std::string basePath = SDL_GetBasePath();
	loadLevel(basePath + "assets/levels/debuglevel.lvl", &level, &enemies, player, &levelImages, debugImage, window);
	std::cout << "Created Objects!" << std::endl;
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
		inputState = 0;
		inputState = inputState | (0b00000001 * (window->keyPressed("Space") || window->keyPressed("W") || window->keyPressed("Up")));
		inputState = inputState | (0b00000010 * (window->keyPressed("S") || window->keyPressed("Down")));
		inputState = inputState | (0b00000100 * (window->keyPressed("A") || window->keyPressed("Left")));
		inputState = inputState | (0b00001000 * (window->keyPressed("D") || window->keyPressed("Right")));
		player->giveInput(inputState);
		while (deltaTimer > PHYSICS_TIMESTEP) {
			deltaTimer -= PHYSICS_TIMESTEP;
			player->step(PHYSICS_TIMESTEP);
			for (ObjectHandler::Object * tile : level) {
				tile->step(PHYSICS_TIMESTEP);
			}
			loops++;
		}
		std::cout << loops << " physics iterations this frame" << std::endl;
		window->clearScreen(new SDLwrapper::Color(10, 200, 255, 255));
		for (ObjectHandler::Object * tile : level) {
			tile->draw(window);
		}
		player->draw(window);
		window->presentWindow();
	}
	window->quit();
	return 0;
}