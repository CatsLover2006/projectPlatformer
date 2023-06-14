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
#include "orc.hpp"
#include "crib.hpp"
#include "door.hpp"

std::ifstream levelFile;
std::string curLine;
std::string curData;
std::vector<double> lineData = {0};
std::string delim = "|";

using namespace ObjectHandler;
using namespace CollisionHandler;
using namespace GameObjects;

SDLwrapper::Image * emptyImg;

bool loadLevel(std::string filename, std::vector<ObjectHandler::Object *>* levelData, std::vector<ObjectHandler::DynamicObject *>* enemyData,
		GameObjects::Player * player, std::vector<SDLwrapper::Image *>* images, std::vector<SDLwrapper::Image *>* enemyImages, SDLwrapper::Image * debugImg, 
		SDLwrapper::Window * window, double bounds[4]) {
	if (levelFile.is_open()) {
		levelFile.close();
		std::cout << "Closed level file." << std::endl;
	}
	std::cout << "Loading level: " << filename << std::endl;
	levelFile.open(filename, std::ifstream::in);
	if (!levelFile.is_open()) {
		std::cout << "Level file could not be loaded..." << std::endl;
		return false;
	}
	std::cout << "Clearing images..." << std::endl;
	for (SDLwrapper::Image * img : *images) {
		if (img != nullptr) {
			delete img;
			img = nullptr;
		}
	}
	for (SDLwrapper::Image * img : *enemyImages) {
		if (img != nullptr) {
			delete img;
			img = nullptr;
		}
	}
	images->clear();
	enemyImages->clear();
	std::cout << "Clearing old level data..." << std::endl;
	for (Object * obj : *levelData) {
		if (obj != nullptr) {
			obj->del();
			obj = nullptr;
		}
	}
	levelData->clear();
	for (DynamicObject * obj : *enemyData) {
		if (obj != nullptr) {
			obj->del();
			obj = nullptr;
		}
	}
	enemyData->clear();
	player->resetPlayer();
	std::cout << "Cleared old level!" << std::endl;
	std::getline(levelFile, curLine);
	delete emptyImg;
	std::cout << "Loading empty texture: " << curLine << std::endl;
	emptyImg = new SDLwrapper::Image(curLine, window);
	std::cout << "Loading level data..." << std::endl;
	std::getline(levelFile, curLine);
	while (curLine != "") {
		curData = curLine.substr(0, curLine.find(delim));
		curLine.erase(0, curLine.find(delim) + delim.length());
		std::cout << "Loading texture: " << curData.substr(0, curData.find(";")) << std::endl;
		SDLwrapper::Image * img = new SDLwrapper::Image(curData.substr(0, curData.find(";")), window);
		if (curData.find(";") + 1) {
			int startIDX = curData.find(";") + 1;
			int endIDX = curData.find(",");
			std::cout << "Resolution: " << curData.substr(startIDX, endIDX-startIDX) << "x" << curData.substr(endIDX+1, -1) << std::endl;
			img->w = stoi(curData.substr(startIDX, endIDX-startIDX));
			img->h = stoi(curData.substr(endIDX+1, -1));
		}
		images->push_back(img);
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
				long *data = new long[2];
				for (int i = 0; i < 2; i++) data[i] = lineData[i + 3];
				levelData->push_back(new Door(lineData[1], lineData[2], images, data, debugImg));
				delete[] data;
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
			case 4: { // Bounded Slope 
				long *data = new long[3];
				for (int i = 0; i < 3; i++) data[i] = lineData[i + 5];
				levelData->push_back(new Slope(lineData[1],lineData[2],lineData[3],lineData[4], images, data, debugImg));
				delete[] data;
				break;
			}
			default: break;
		}
	}
	lineData.clear();
	std::cout << "Loaded level data!" << std::endl << "Loading enemy data..." << std::endl;
	std::getline(levelFile, curLine);
	while (curLine != "") {
		curData = curLine.substr(0, curLine.find(delim));
		curLine.erase(0, curLine.find(delim) + delim.length());
		std::cout << "Loading texture: " << curData << std::endl;
		enemyImages->push_back(new SDLwrapper::Image(curData, window));
	}
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
			case 0: {
				long data[9];
				for (int i = 0; i < 9; i++) data[i] = lineData[i + 3];
				enemyData->push_back(new Orc(lineData[1], lineData[2], enemyImages, data, player, debugImg));
				break;
			}
			case 1: {
				long data[9];
				for (int i = 0; i < 9; i++) data[i] = lineData[i + 3];
				enemyData->push_back(new FastOrc(lineData[1], lineData[2], enemyImages, data, player, debugImg));
				break;
			}
			case 2: {
				long data[9];
				for (int i = 0; i < 9; i++) data[i] = lineData[i + 3];
				enemyData->push_back(new SmartOrc(lineData[1], lineData[2], enemyImages, data, player, debugImg));
				break;
			}
			default: break;
		}
	}
	lineData.clear();
	std::cout << "Loaded enemy data!" << std::endl << "Loading extra level data..." << std::endl;
	std::getline(levelFile, curLine);
	while (curLine != "") {
		curData = curLine.substr(0, curLine.find(delim));
		curLine.erase(0, curLine.find(delim) + delim.length());
		lineData.push_back(std::stod(curData));
	}
	for (int i = 0; i < 4; i++) bounds[i] = lineData[i];
	std::cout << "Loaded level!" << std::endl;
	levelFile.close();
	return true;
}