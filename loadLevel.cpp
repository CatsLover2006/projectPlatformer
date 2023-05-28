#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include "hailLib/sdlWrapper.hpp"
#include "hailLib/basemath.hpp"
#include "collider.hpp"
#include "object.hpp"
#include "player.hpp"

std::ifstream levelFile;
std::string curLine;
std::string curData;
std::vector<long> lineData = {0};
const std::string delim = "|";

using namespace ObjectHandler;
using namespace CollisionHandler;

void loadLevel(std::string filename, std::vector<ObjectHandler::Object *>* levelData,
		std::vector<ObjectHandler::DynamicObject *>* enemyData, std::vector<SDLwrapper::Image *>* images, SDLwrapper::Image * debugImg, SDLwrapper::Window * window) {
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
		std::cout << lineData[0] << std::endl;
		switch (lineData[0]) {
			case 2: {
				long *data = new long[9];
				for (int i = 0; i < 9; i++) data[i] = lineData[i + 5];
				levelData->push_back(new GroundObject(new BoxCollider(lineData[1],lineData[2],lineData[3],lineData[4]), images, data, debugImg));
				delete[] data;
				break;
			}
			default: break;
		}
	}
	lineData.clear();
	lineData.push_back(0);
	lineData.shrink_to_fit();
	levelFile.close();
}