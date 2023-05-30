#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include "hailLib/sdlWrapper.hpp"
#include "hailLib/basemath.hpp"
#include "collider.hpp"
#include "object.hpp"
#include "playerInteractableObjects.hpp"
#include "player.hpp"
#include "crib.hpp"

std::ifstream levelFile;
std::string curLine;
std::string curData;
std::vector<double> lineData = {0};
const std::string delim = "|";

using namespace ObjectHandler;
using namespace CollisionHandler;
using namespace GameObjects;

SDLwrapper::Image * emptyImg;

void loadLevel(std::string filename, std::vector<ObjectHandler::Object *>* levelData, std::vector<ObjectHandler::DynamicObject *>* enemyData,
		GameObjects::Player * player, std::vector<SDLwrapper::Image *>* images, SDLwrapper::Image * debugImg, SDLwrapper::Window * window, double bounds[4]) {
	if (levelFile.is_open()) levelFile.close();
	levelFile.open(filename, std::ifstream::in);
	if (!levelFile.is_open()) {
		std::cout << "Level file could not be loaded..." << std::endl;
		return;
	}
	for (SDLwrapper::Image * img : *images) {
		if (img != nullptr) {
			delete img;
			img = nullptr;
		}
	}
	images->clear();
	for (Object * obj : *levelData) {
		if (obj != nullptr) {
			delete obj;
			obj = nullptr;
		}
	}
	levelData->clear();
	for (DynamicObject * obj : *enemyData) {
		if (obj != nullptr) {
			delete obj;
			obj = nullptr;
		}
	}
	enemyData->clear();
	player->hasBaby = false;
	std::getline(levelFile, curLine);
	delete emptyImg;
	emptyImg = new SDLwrapper::Image(curLine, window);
	std::getline(levelFile, curLine);
	while (curLine != "") {
		curData = curLine.substr(0, curLine.find(delim));
		curLine.erase(0, curLine.find(delim) + delim.length());
		std::cout << "Loading texture: " << curData << std::endl;
		images->push_back(new SDLwrapper::Image(curData, window));
	}
	while (lineData[0] != -1) {
		lineData.clear();
		std::getline(levelFile, curLine);
		while (curLine != "") {
			curData = curLine.substr(0, curLine.find(delim));
			curLine.erase(0, curLine.find(delim) + delim.length());
			lineData.push_back(std::stod(curData));
		}
		std::cout << "Loading Object ID: " << lineData[0] << std::endl;
		switch ((long)lineData[0]) {
			case 0: {
				player->collision->x = lineData[1] + 8;
				player->collision->y = lineData[2];
				player->vX = 0;
				player->vY = 0;
				// TODO: spawn door
				break;
			}
			case 1: {
				long *data = new long[3];
				for (int i = 0; i < 3; i++) data[i] = lineData[i + 3];
				levelData->push_back(new Crib(lineData[1], lineData[2], images, data, debugImg));
				delete[] data;
				break;
			}
			case 2: {
				long *data = new long[9];
				for (int i = 0; i < 9; i++) data[i] = lineData[i + 5];
				levelData->push_back(new GroundObject(new BoxCollider(lineData[1],lineData[2],lineData[3],lineData[4]), images, data, debugImg));
				delete[] data;
				break;
			}
			case 3: {
				long *data = new long[3];
				for (int i = 0; i < 3; i++) data[i] = lineData[i + 5];
				long *vEndData = new long[9];
				for (int i = 0; i < 9; i++) vEndData[i] = -1;
				levelData->push_back(new GroundObject(new BoxCollider(lineData[1],hailMath::max<double>(lineData[2], lineData[2]+lineData[4]),lineData[3], 0), images, vEndData, emptyImg)); // Bug fix: phasing through corners
				levelData->push_back(new GroundObject(new BoxCollider((lineData[4]<0)?lineData[1]+lineData[3]:lineData[1],hailMath::min<double>(lineData[2], lineData[2]+lineData[4]),0, hailMath::abs_q((double)lineData[4])), images, vEndData, emptyImg));
				levelData->push_back(new Slope(lineData[1],lineData[2],lineData[3],lineData[4], images, data, debugImg)); // Bug fix: clipping through slopes vertically
				delete[] data;
				delete[] vEndData;
				break;
			}
			default: break;
		}
	}
	lineData.clear();
	lineData.push_back(0);
	while (lineData[0] != -1) {
		lineData.clear();
		std::getline(levelFile, curLine);
		while (curLine != "") {
			curData = curLine.substr(0, curLine.find(delim));
			curLine.erase(0, curLine.find(delim) + delim.length());
			lineData.push_back(std::stod(curData));
		}
		std::cout << "Loading Dynamic Object ID: " << lineData[0] << std::endl;
		switch ((long)lineData[0]) {
			default: break;
		}
	}
	lineData.clear();
	std::getline(levelFile, curLine);
	while (curLine != "") {
		curData = curLine.substr(0, curLine.find(delim));
		curLine.erase(0, curLine.find(delim) + delim.length());
		lineData.push_back(std::stod(curData));
	}
	for (int i = 0; i < 4; i++) bounds[i] = lineData[i];
	levelFile.close();
}