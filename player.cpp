#include <string> // Error fixing
#include <iostream>
#include <vector>
#include <typeinfo>
#include "collider.hpp"
#include "hailLib/basemath.hpp"
#include "hailLib/sdlWrapper.hpp"
#include "object.hpp"
#include "player.hpp"
#define PLAYER_ACCEL 700
#define PLAYER_FRICTION 1000
#define PLAYER_SPEED 222

double abs_q(double a) {
	uint64_t b = *((uint64_t*)&a);
	b = b & 0x7fffffffffffffff; // Mask signiture bit
	double c = *((double*)&b);
	return c;
}

namespace GameObjects {

	Player::Player (double x, double y, std::vector<SDLwrapper::Image *> sprites): ObjectHandler::DynamicObject(new CollisionHandler::BoxCollider(x, y, 24, 32), 400, sprites) {};
	void Player::giveInput (uint8_t input) {
		inputState = input;
	}
	void Player::step(double deltaTime) {
		vY += grav * deltaTime;
		if (inputState & 0b00000001) {
			std::cout << "Jump Attempt" << std::endl;
			collision->y += 4;
			for (ObjectHandler::Object * obj : objectList) {
				if (obj == this) continue;
				if (collision->colliding(obj->collision)) {
					vY = -200;
					std::cout << "Jump" << std::endl;
				}
			}
			collision->y -= 4;
		}
		bool t_disabled = false;
		if (inputState & 0b00001000) {
			if (vX < PLAYER_SPEED) vX += deltaTime * PLAYER_ACCEL;
			else vX = PLAYER_SPEED;
			t_disabled = true;
		} else if (vX > 0) vX -= deltaTime * PLAYER_FRICTION;
		if (inputState & 0b00000100) {
			if (vX > -PLAYER_SPEED) vX -= deltaTime * PLAYER_ACCEL;
			else vX = -PLAYER_SPEED;
			t_disabled = true;
		} else if (vX < 0) vX += deltaTime * PLAYER_FRICTION;
		if (hailMath::abs(vX) < 10 && !t_disabled) vX = 0; // 25 pixels per frame is nothing
		collision->y -= 4;
		collision->x += vX * deltaTime;
		for (ObjectHandler::Object * obj : objectList) {
			if (obj == this) continue;
			if (collision->colliding(obj->collision)) {
				unintersectX(obj);
				vX = 0;
			}
		}
		collision->y += vY * deltaTime + 4;
		for (ObjectHandler::Object * obj : objectList) {
			if (obj == this) continue;
			if (collision->colliding(obj->collision)) {
				unintersectY(obj);
				vY = 0;
			}
		}
	}
	void Player::draw(SDLwrapper::Window * window) {
		// TODO: animate
		for (SDLwrapper::Image * image : sprites) window->drawImage(image, collision->getBound_l(), collision->getBound_t(), 
																	collision->getBound_r() - collision->getBound_l(), collision->getBound_b() - collision->getBound_t());
	}
}