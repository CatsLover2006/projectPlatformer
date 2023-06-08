#include <typeinfo>
#include <cmath>
#include <iostream>
#include <vector>
#include "collider.hpp"
#include "hailLib/basemath.hpp"
#include "hailLib/sdlWrapper.hpp"
#include "object.hpp"
#include "playerInteractableObjects.hpp"
#include "door.hpp"
#include "player.hpp"

namespace GameObjects {
	Door::Door (double x, double y, std::vector<SDLwrapper::Image *> * sprites, long spriteIndexes[2], SDLwrapper::Image * debugImage): InteractableObject(new CollisionHandler::BoxCollider(x, y, 32, 32)){
		this->sprites = sprites;
		for (int i = 0; i < 3; i++) this->spriteIndexes[i] = spriteIndexes[i];
		this->debugImage = debugImage;
		backToDoor = false;
		t = 0;
	}

	void Door::step(double deltaTime) {
		t += deltaTime;
	}

	SDLwrapper::Image * Door::getImageAtPos(long in) {
		if (spriteIndexes[in] == -1) return debugImage;
		else {
			return sprites->at(spriteIndexes[in]);
		}
	}

	void Door::draw (SDLwrapper::Window * window) {
		if (t > 0.1) window->drawImage(getImageAtPos(0), collision->x, collision->y, 32, 32);
		else window->drawImage(getImageAtPos(1), collision->x, collision->y, 32, 32);
	}

	void Door::handleInteraction(Player * player) {
		if (player->hasBaby) {
			backToDoor = true;
			t = 0;
		}
	}
}