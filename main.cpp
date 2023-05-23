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
#include "player.hpp"

#define PHYSICS_TIMESTEP 0.005

std::chrono::high_resolution_clock::time_point startFrame, endFrame;
std::chrono::duration<double> deltaTime;

double w = 200;
double h = 200;
double deltaTimer = 0;
double angle = 0;
const double pi = std::atan(1.0)*4;
int loops;

std::vector<ObjectHandler::Object *> level;
std::vector<ObjectHandler::DynamicObject *> enemies;
GameObjects::Player * player;
uint8_t inputState;

std::vector<SDLwrapper::Image *> groundSprites;
SDLwrapper::Image * debugImage;

double degToRad(double deg) {return deg*pi/180;}

int main() {
	std::cout << "Physics timestep is " << PHYSICS_TIMESTEP << "s" << std::endl;
	SDLwrapper::Window * window = new SDLwrapper::Window(640, 480, "Project Platformer");
	std::cout << "Loading Game Data..." << std::endl;
	debugImage = new SDLwrapper::Image("assets/textures/debug.gif", window);
	groundSprites.push_back(new SDLwrapper::Image("assets/textures/World1Ground/TopLeft.gif", window));
	groundSprites.push_back(new SDLwrapper::Image("assets/textures/World1Ground/Top.gif", window));
	groundSprites.push_back(new SDLwrapper::Image("assets/textures/World1Ground/TopRight.gif", window));
	groundSprites.push_back(debugImage);
	groundSprites.push_back(new SDLwrapper::Image("assets/textures/World1Ground/Mid.gif", window));
	{
		std::vector<SDLwrapper::Image *> playerImages;
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
	}
	std::cout << "Loaded Game Data!" << std::endl;
	std::cout << "Creating Objects..." << std::endl;
	{
		std::vector<SDLwrapper::Image *> tempImg;
		tempImg.push_back(groundSprites[0]);
		tempImg.push_back(groundSprites[1]);
		tempImg.push_back(groundSprites[2]);
		tempImg.push_back(groundSprites[3]);
		tempImg.push_back(groundSprites[4]);
		tempImg.push_back(debugImage);
		tempImg.push_back(debugImage);
		tempImg.push_back(debugImage);
		tempImg.push_back(debugImage);
		level.push_back(new ObjectHandler::GroundObject(new CollisionHandler::BoxCollider(100, 100, 288, 288), tempImg, 0));
	}
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
		// TODO: pass input to player
		std::cout << window->keysDown.size() << " keys pressed" << std::endl;
		while (deltaTimer > PHYSICS_TIMESTEP) {
			deltaTimer -= PHYSICS_TIMESTEP;
			player->step(PHYSICS_TIMESTEP);
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