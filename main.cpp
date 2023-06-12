#include <iostream>
#include <fstream>
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
double waitTimer = 0;
int menuScroll = 0;
int loops;

uint8_t men, oMen;

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
SDLwrapper::Font * lrgMenuFont;
SDLwrapper::Font * medMenuFont;
SDLwrapper::Font * smlMenuFont;

std::string curLvl;
std::string basePath;

bool lost = false;

gameState state = MENU;
gameState oldState = MENU;
bool stateSwap = false;

class Level {
	public:
		std::string name;
		std::string path;
		Level(std::string name, std::string path) {
			this->name = name;
			this->path = path;
		};
		Level() {
			name = "";
			path = "";
		};
};

std::vector<Level *> levelList;
std::ifstream lstFile;

void loadLevelList(std::string filename) {
	if (lstFile.is_open()) {
		lstFile.close();
		std::cout << "Closed level list file." << std::endl;
	}
	lstFile.open(filename, std::ifstream::in);
	if (!lstFile.is_open()) {
		std::cout << "Level file could not be loaded..." << std::endl;
		return;
	}
	std::string curLine, curData, delim = "|";
	std::vector<std::string> lineData;
	lineData.push_back(" ");
	std::cout << "Loading level list: " << filename << std::endl;
	if (levelList.size() != 0) {
		std::cout << "Clearing old level list..." << std::endl;
		for (Level * lvl : levelList) {
			if (lvl) {
				delete lvl;
				lvl = nullptr;
			}
		}
	}
	levelList.clear();
	std::getline(lstFile, curLine);
	while (lineData[0] != "") {
		lineData.clear();
		if (curLine == "") break;
		while (curLine != "") {
			curData = curLine.substr(0, curLine.find(delim));
			curLine.erase(0, curLine.find(delim) + delim.length());
			lineData.push_back(curData);
		}
		levelList.push_back(new Level(lineData.at(0), lineData.at(1)));
		std::getline(lstFile, curLine);
	}
	std::cout << "Loaded level list!" << std::endl;
	lstFile.close();
}

int main() {
	std::cout << "Physics timestep is " << PHYSICS_TIMESTEP << "s" << std::endl;
	std::cout << "Creating window..." << std::endl;
	SDLwrapper::Window * window = new SDLwrapper::Window(864, 480, "Project Platformer");
	std::cout << "Loading Game Data..." << std::endl;
	debugImage = new SDLwrapper::Image("assets/textures/debug.gif", window);
	lrgMenuFont = new SDLwrapper::Font("assets/fonts/Geologica-Medium.ttf", 64);
	medMenuFont = new SDLwrapper::Font("assets/fonts/Geologica-Medium.ttf", 32);
	smlMenuFont = new SDLwrapper::Font("assets/fonts/Geologica-Medium.ttf", 16);
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Body_Nu.gif", window)); 
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Head.gif", window));
	playerImages.push_back(debugImage);
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Arm_Top_Nu.gif", window));
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Arm_Bottom_Nu.gif", window));
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Leg_Top_Nu.gif", window));
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Leg_Bottom_Nu.gif", window));
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Arm_Bottom_Nu.gif", window)); // Carry
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Leg_Bottom_Step_Nu.gif", window));
	playerImages.push_back(new SDLwrapper::Image("assets/textures/Player/Elbow_Kneecap.gif", window));
	player = new GameObjects::Player(200, -10, playerImages);
	std::cout << "Loaded Game Data!" << std::endl;
	std::cout << "Creating Objects..." << std::endl;
	basePath = SDL_GetBasePath();
	loadLevelList(basePath + "assets/levels/levels.lst");
	curLvl = basePath + levelList.at(0)->path;
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
		oldState = state;
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
					if (player->collision->y > bounds[3] && !player->dead) player->kill();
					if (player->dead && player->deadTmr <= 0) {
						lost = true;
						state = FINISHEDLEVEL;
						break;
					}
					loops++;
				}
				break;
			}
			case FINISHEDLEVEL: {
				if (stateSwap) { // First run
					waitTimer = 0;
				}
				while (deltaTimer > PHYSICS_TIMESTEP) {
					deltaTimer -= PHYSICS_TIMESTEP;
					waitTimer += PHYSICS_TIMESTEP;
					if (waitTimer >= 1) {
						state = MENU;
						break;
					}
					loops++;
				}
				break;
			}
			case MENU: {
				men = 0;
				men = men | ((window->keyPressed("S") || window->keyPressed("Down")) * 0b00000001);
				men = men | ((window->keyPressed("W") || window->keyPressed("Up")) * 0b00000010);
				if (menuScroll > 0 && ((men & oMen) & 0b00000010)) {
					menuScroll--;
					curLvl = basePath + levelList.at(menuScroll)->path;
				}
				if (menuScroll < (levelList.size() - 1) && ((men & oMen) & 0b00000001)) {
					menuScroll++;
					curLvl = basePath + levelList.at(menuScroll)->path;
				}
				if (window->keyPressed("M")) {
					loadLevel(curLvl, &level, &enemies, player, &levelImages, &enemyImages, debugImage, window, bounds);
					state = INGAME;
					break;
				}
				oMen = ~men;
				while (deltaTimer > PHYSICS_TIMESTEP) {
					deltaTimer -= PHYSICS_TIMESTEP;
					loops++;
				}
				break;
			}
		}
		stateSwap = state != oldState;
		std::cout << loops << " physics iterations this frame" << std::endl;
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
				window->clearScreen(new SDLwrapper::Color(255, 255, 255, 255));
				if (lost) {
					window->drawTextCentered("RIP L", lrgMenuFont, new SDLwrapper::Color(0, 0, 0, 255), 432, 240);
				} else {
					window->drawTextCentered("GG!", lrgMenuFont, new SDLwrapper::Color(0, 0, 0, 255), 432, 240);
				}
				break;
			}
			case MENU: {
				window->clearScreen(new SDLwrapper::Color(255, 255, 255, 255));
				window->translate(0, -18 * menuScroll);
				for (int i = 0; i < levelList.size(); i++) {
					window->drawTextCentered(levelList.at(i)->name, smlMenuFont, new SDLwrapper::Color(0, 0, i == menuScroll?255:0, 255), 432, 240);
					window->translate(0, 18);
				}
				window->resetTranslation();
				window->drawRect(new SDLwrapper::Color(0, 0, 0, 255), 0, 0, 864, 64);
				window->drawTextCentered("Press M to play level.", medMenuFont, new SDLwrapper::Color(255, 255, 255, 255), 432, 32);
				break;
			}
		}
		window->resetTranslation();
		window->drawRect(new SDLwrapper::Color(0, 0, 0), window->mouseX-2.5, window->mouseY-2.5, 5, 5);
		window->presentWindow();
	}
	window->quit();
	return 0;
}