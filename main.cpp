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
#include "orc.hpp"
#include "player.hpp"
#include "loadLevel.hpp"
#include "door.hpp"

enum gameState {
	INGAME = 1,
	MENU = 0,
	FINISHEDLEVEL = 2
};

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

double bounds[4];

double camera_x, camera_y;

std::vector<SDLwrapper::Image *> levelImages;
std::vector<SDLwrapper::Image *> enemyImages;
SDLwrapper::Image * debugImage;

std::string curLvl;
std::string basePath;

bool lost = false;

gameState state = MENU;

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
	basePath = SDL_GetBasePath();
	curLvl = basePath + "assets/levels/debuglevel.lvl";
	camera_x = 0;
	camera_y = 0;
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
		deltaTimer += deltaTime.count();
		loops = 0;
		window->runInput();
		switch (state) {
			case INGAME: {
				inputState = 0;
				inputState = inputState | (0b00000001 * (window->keyPressed("Space") || window->keyPressed("W") || window->keyPressed("Up")));
				inputState = inputState | (0b00000010 * (window->keyPressed("S") || window->keyPressed("Down")));
				inputState = inputState | (0b00000100 * (window->keyPressed("A") || window->keyPressed("Left")));
				inputState = inputState | (0b00001000 * (window->keyPressed("D") || window->keyPressed("Right")));
				player->giveInput(inputState);
				while (deltaTimer > PHYSICS_TIMESTEP) {
					deltaTimer -= PHYSICS_TIMESTEP;
					player->preStep(PHYSICS_TIMESTEP);
					for (ObjectHandler::DynamicObject * object : enemies) {
						object->preStep(PHYSICS_TIMESTEP);
					}
					player->step(PHYSICS_TIMESTEP);
					for (ObjectHandler::Object * tile : level) {
						tile->step(PHYSICS_TIMESTEP);
						GameObjects::Door * door = dynamic_cast<GameObjects::Door*>(tile);
						if (door) {
							if (!door->backToDoor) continue;
							lost = false;
							state = FINISHEDLEVEL;
							break;
						}
					}
					for (ObjectHandler::DynamicObject * object : enemies) {
						object->step(PHYSICS_TIMESTEP);
					}
					camera_x += (player->collision->x - 432) * 0.1;
					camera_x /= 1.1;
					camera_x = hailMath::constrain<double>(camera_x, bounds[0], bounds[2] - 864);
					camera_y += (player->collision->y - 352) * 0.1;
					camera_y /= 1.1;
					camera_y = hailMath::constrain<double>(camera_y, bounds[1], bounds[3] - 480);
					if (player->collision->y > bounds[3]) {
						lost = true;
						state = FINISHEDLEVEL;
						break;
					}
					loops++;
				}
				break;
			}
			case FINISHEDLEVEL: {

				while (deltaTimer > PHYSICS_TIMESTEP) {
					deltaTimer -= PHYSICS_TIMESTEP;
				}
				break;
			}
			case MENU: {
				loadLevel(curLvl, &level, &enemies, player, &levelImages, &enemyImages, debugImage, window, bounds); // Reset
				while (deltaTimer > PHYSICS_TIMESTEP) {
					deltaTimer -= PHYSICS_TIMESTEP;
				}
				state = INGAME;
				break;
			}
		}
		std::cout << loops << " physics iterations this frame" << std::endl;
		window->resetTranslation();
		switch (state) {
			case INGAME: {
				window->clearScreen(new SDLwrapper::Color(10, 200, 255, 255));
				window->translate(-camera_x, -camera_y);
				for (ObjectHandler::Object * tile : level) {
					tile->draw(window);
				}
				for (ObjectHandler::DynamicObject * object : enemies) {
					object->draw(window);
				}
				player->draw(window);
				break;
			}
			case FINISHEDLEVEL: {
				if (lost) std::cout << "RIP L" << std::endl;
				else std::cout << "GG!" << std::endl;
				break;
			}
			case MENU: {
				window->clearScreen(new SDLwrapper::Color(255, 255, 255, 255));
				break;
			}
		}
		window->presentWindow();
	}
	window->quit();
	return 0;
}