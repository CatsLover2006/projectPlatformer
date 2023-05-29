#include <typeinfo>
#include <cmath>
#include <iostream>
#include <vector>
#include "collider.hpp"
#include "hailLib/basemath.hpp"
#include "hailLib/sdlWrapper.hpp"
#include "object.hpp"
#include "playerInteractableObjects.hpp"
#include "crib.hpp"
#include "player.hpp"

namespace GameObjects {
	Crib::Crib (double x, double y, std::vector<SDLwrapper::Image *> * sprites, long spriteIndexes[3], SDLwrapper::Image * debugImage): InteractableObject(new CollisionHandler::BoxCollider(x, y, 32, 32)){
		this->sprites = sprites;
		for (int i = 0; i < 3; i++) this->spriteIndexes[i] = spriteIndexes[i];
		this->debugImage = debugImage;
		hasBaby = true;
		babyAngle = 0;
	}

	void Crib::step(double deltaTime) {
		if (reverseBabyAngle) {
			babyAngle += deltaTime * 20;
			if (babyAngle > 20) {
				babyAngle = 40 - babyAngle;
				reverseBabyAngle = false;
			}
		} else {
			babyAngle -= deltaTime * 20;
			if (babyAngle < -20) {
				babyAngle = -40 - babyAngle;
				reverseBabyAngle = true;
			}
		}
	}

	SDLwrapper::Image * Crib::getImageAtPos(long in) {
		if (spriteIndexes[in] == -1) return debugImage;
		else {
			return sprites->at(spriteIndexes[in]);
		}
	}

	void Crib::draw (SDLwrapper::Window * window) {
		window->drawImage(getImageAtPos(0), collision->x, collision->y, 32, 32);
		if (hasBaby) window->drawImageEx(getImageAtPos(1), collision->x + 13.5, collision->y + 10, 7, 13.0, 90 + babyAngle);
		window->drawImage(getImageAtPos(2), collision->x, collision->y, 32, 32);
	}

	void Crib::handleInteraction(Player * player) {
		if (!hasBaby) return;
		player->hasBaby = true;
		player->setImage(0, getImageAtPos(1));
		hasBaby = false;
	}
}