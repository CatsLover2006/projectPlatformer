#include <string> // Error fixing
#include <iostream>
#include <vector>
#include <typeinfo>
#include "collider.hpp"
#include "sdlWrapper.hpp"
#include "object.hpp"
#include "player.hpp"
#define PLAYER_ACCEL 100
#define PLAYER_FRICTION 40
#define PLAYER_SPEED 50

namespace GameObjects {
	Player::Player (double x, double y, std::vector<SDLwrapper::Image *> sprites): ObjectHandler::DynamicObject(new CollisionHandler::BoxCollider(x, y, 24, 32), 100, sprites) {}; // Nothing else to do lol
	void Player::giveInput (uint8_t input) {
		inputState = input;
	}
	void Player::step(double deltaTime) {
		vY += grav * deltaTime;
		if (inputState & 0b00001000) {
			if (vX < PLAYER_SPEED) vX += deltaTime * PLAYER_ACCEL;
		} else if (vX > 0) vX -= deltaTime * PLAYER_FRICTION;
		if (inputState & 0b00000100) {
			if (vX > -PLAYER_SPEED) vX -= deltaTime * PLAYER_ACCEL;
		} else if (vX < 0) vX += deltaTime * PLAYER_FRICTION;
		collision->y += vY * deltaTime;
		collision->x += vX * deltaTime;
	}
	void Player::draw(SDLwrapper::Window * window) {
		// TODO: animate
		for (SDLwrapper::Image * image : sprites) window->drawImage(image, collision->getBound_l(), collision->getBound_t(), collision->getBound_r() - collision->getBound_l(), collision->getBound_b() - collision->getBound_t());
	}
}