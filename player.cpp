#include <string> // Error fixing
#include <iostream>
#include <vector>
#include <typeinfo>
#include <cmath>
#include "collider.hpp"
#include "hailLib/basemath.hpp"
#include "hailLib/sdlWrapper.hpp"
#include "object.hpp"
#include "playerInteractableObjects.hpp"
#include "player.hpp"
#define PLAYER_ACCEL 1400
#define PLAYER_FRICTION 3000
#define PLAYER_SPEED 444
#define JUMP_HEIGHT 400
#define GRAVITY 800
		
double sOX, tSOX;
double sOY, tSOY;
double progress;
int flipMult;
double tX, tY, cX, cY, aX, aY, bX, bY, mX, mY;
bool stepIn, stepOut;
bool dontReset;
bool overLoad;
bool canJump;

namespace GameObjects {
	bool hadJumped;
	bool hadPressed;
	bool Player::checkAnimNoset() {
		if (anim == 3) return true;
		return false;
	}

	Player::Player (double x, double y, std::vector<SDLwrapper::Image *> sprites): ObjectHandler::DynamicObject(new CollisionHandler::BoxCollider(x, y, 24, 32), GRAVITY, sprites) {
		//hasBaby = true;
	}

	void Player::giveInput (uint8_t input) {
		inputState = input;
	}

	bool Player::checkCanJump() {
		collision->y += 4;
		for (ObjectHandler::Object * obj : objectList) {
			if (obj == this) continue;
			if (obj->isTrigger) continue; 
			if (collision->colliding(obj->collision)) {
				collision->y -= 4;
				return true;
			}
		}
		collision->y -= 4;
		return false;
	}

	void Player::step(double deltaTime) {
		animProgress += deltaTime;
		vY += grav * deltaTime;
		if (!checkAnimNoset()) anim = 0;
		if (!hadPressed) hadJumped = false;
		bool t_disabled = false;
		if (inputState & 0b00001000) {
			if (vX < PLAYER_SPEED) vX += deltaTime * PLAYER_ACCEL;
			else vX = PLAYER_SPEED;
			t_disabled = true;
			flipped = false;
			if (checkAnimNoset()) goto pastRight;
			anim = 1;
			if (vX < 0) anim = 4;
		} else if (vX > 0) vX -= deltaTime * PLAYER_FRICTION;
		pastRight:
		if (inputState & 0b00000100) {
			if (vX > -PLAYER_SPEED) vX -= deltaTime * PLAYER_ACCEL;
			else vX = -PLAYER_SPEED;
			t_disabled = true;
			flipped = true;
			if (checkAnimNoset()) goto pastLeft;
			anim = 1;
			if (vX > 0) anim = 4;
		} else if (vX < 0) vX += deltaTime * PLAYER_FRICTION;
		pastLeft:
		canJump = checkCanJump();
		if (canJump) coyote = 14;
		else if (coyote) coyote--;
		hadPressed = inputState & 0b00000001;
		if (coyote) {
			canWallJump = true;
			if (inputState & 0b00000001) {
				if (hadPressed && hadJumped) goto skipJump;
				hadJumped = true;
				vY = -JUMP_HEIGHT;
				anim = 3;
			} else if (anim == 3) anim = 0;
		} else if (vY < 0 && anim == 3) anim = 3;
		else anim = 2;
		skipJump:
		if (hailMath::abs_q(vX) < 20 && !t_disabled) vX = 0; // 20 pixels per second is nothing
		collision->y -= 4;
		dontReset = false;
		tX = vY;
		vY = -4;
		for (ObjectHandler::Object * obj : objectList) {
			if (obj == this) continue;
			if (obj->isTrigger) continue;
			if (collision->colliding(obj->collision)) {
				unintersectY(obj);
				dontReset = true;
			}
		}
		if (dontReset) {
			vY = 0;
			collision->y += 4;
		} else vY = tX;
		collision->x += vX * deltaTime;
		dontReset = false;
		for (ObjectHandler::Object * obj : objectList) {
			if (obj == this) continue;
			if (obj->isTrigger) continue;
			if (collision->colliding(obj->collision)) {
				unintersectX(obj);
				dontReset = true;
			}
		}
		if (dontReset) {
			if (canWallJump && !hasBaby && !canJump) anim = 5;
			wallCoyote = 30;
			wallDir = (vX/hailMath::abs_q(vX));
			vX = 0;
		} else if (wallCoyote) wallCoyote--;
		if (!hasBaby && wallCoyote && canWallJump && !(hadPressed && hadJumped) && inputState & 0b00000001) {
			vY = -JUMP_HEIGHT;
			vX = wallDir * -PLAYER_SPEED;
			anim = 3;
			canWallJump = false;
		}
		collision->y += vY * deltaTime + 4;
		if (vY > 0) {
			overLoad = false;
			dontReset = false;
			for (ObjectHandler::Object * obj : objectList) {
				if (obj == this) continue;
				if (obj->isTrigger) continue; 
				if (collision->colliding(obj->collision)) {
					unintersectY(obj);
					dontReset = true;
					if (obj->collision->y > collision->y) overLoad = true;
				}
			}
			if (dontReset) {
				if (overLoad) vY = hailMath::min<double>(vY, 0);
				else vY = 0;
			}
		}
	triggerCheck:
		// Handle triggers
		for (ObjectHandler::Object * obj : objectList) {
			if (obj == this) continue;
			if (!obj->isTrigger) continue; 
			if (collision->colliding(obj->collision)) {
				InteractableObject * object = (InteractableObject*) obj; // We know it's an interactable object
				object->handleInteraction(this);
			}
		}
		updateLimbPos();
	}

	enum {
		BODY = 0,
		HEAD = 1,
		HAIR = 2,
		ARM = 3,
		FOREARM = 4,
		LEG = 5,
		CALF = 6,
		FOREARM_CARRY = 7,
		CALF_STEP = 8
	} player_sprite_lst;

	int Player::getFlipMult() {
		if (flipped) return -1;
		return 1;
	}

	void Player::updateLimbPos() {
		// Normal animation
		switch (anim) {
			case 4: { // Turnaround; Static Pose
				tSOY = 5;
				tAngles[0] = 10;
				tAngles[1] = -10;
				tAngles[3] = -20;
				tAngles[4] = -30;
				tAngles[5] = -acos((15 - tSOY)/15)*(180/hailMath::pi);
				tAngles[6] = -2 * tAngles[5];
				tAngles[7] = 0;
				tAngles[8] = 0;
				tAngles[9] = 35;
				tAngles[10] = 0;
				stepIn = true;
				stepOut = true;
				animProgress = 0.125;
				break;
			}
			case 3: { // Jump; Static Pose
				tSOY = 0;
				tAngles[0] = -10;
				tAngles[1] = -10;
				tAngles[3] = -130;
				tAngles[4] = -30;
				tAngles[5] = -70;
				tAngles[6] = 70;
				tAngles[7] = 70;
				tAngles[8] = -15;
				tAngles[9] = 30;
				tAngles[10] = 0;
				stepIn = false;
				stepOut = false;
				animProgress = 0;
				break;
			}
			case 5: { // Airfall-slide
				tSOY = 0;
				tAngles[0] = 4;
				tAngles[1] = -40;
				tAngles[3] = -84;
				tAngles[4] = -100;
				tAngles[5] = -acos(14.0/15)*(180/hailMath::pi);
				tAngles[6] = 0;
				tAngles[7] = 90;
				tAngles[8] = -10;
				tAngles[9] = -acos(10.0/15)*(180/hailMath::pi);
				tAngles[10] = 0;
				stepIn = false;
				stepOut = false;
				animProgress = 0.25;
				break;
			}
			case 2: { // Airfall
				tSOY = 0;
				tAngles[0] = 4;
				tAngles[1] = hailMath::constrain<double>(vY / 5, -40, 40);
				tAngles[3] = 70;
				tAngles[4] = -10;
				tAngles[5] = -acos(14.0/15)*(180/hailMath::pi);
				tAngles[6] = 0;
				tAngles[7] = 90;
				tAngles[8] = -10;
				tAngles[9] = -acos(10.0/15)*(180/hailMath::pi);
				tAngles[10] = 0;
				stepIn = false;
				stepOut = false;
				animProgress = 0.25;
				break;
			}
			case 1: { // Running
				progress = fmod(animProgress / 1.2, .5);
				tSOY = hailMath::abs_q(fmod(progress + .125, .25)-.125) * 30 - 5;
				tAngles[0] = 30;
				tAngles[1] = -30;
				tAngles[3] = -tAngles[0] - hailMath::abs_q(fmod(progress + .25, .5)-.25) * 100 * 4 + 60;
				tAngles[4] = -hailMath::abs_q(fmod(progress + .25, .5)-.25) * 150 * 4 + 20;
				tAngles[5] = 80 * cos(progress * hailMath::pi * 4) + 10;
				tAngles[6] = 70 + 70 * sin(progress * hailMath::pi * 4);
				tAngles[7] = -tAngles[0] + hailMath::abs_q(fmod(.75 - progress, .5)-.25) * 100 * 4 - 40;
				tAngles[8] = -hailMath::abs_q(fmod(progress, .5)-.25) * 150 * 4 + 20;
				tAngles[9] = 10 - 80 * cos(progress * hailMath::pi * 4);
				tAngles[10] = 70 + 70 * sin(-progress * hailMath::pi * 4);
				stepIn = progress > 0.15 && progress < 0.2;
				stepOut = progress > 0.4 && progress < 0.45;
				break;
			}
			case 0: // Standing
			default: { // IDK what this is, just stand
				progress = fmod(animProgress, 2.0);
				tAngles[0] = sin(hailMath::pi * progress) + 1.2;
				tSOY = tAngles[0] / 4;
				tAngles[1] = -tAngles[0];
				tAngles[3] = tAngles[1];
				tAngles[4] = tAngles[1];
				tAngles[5] = -acos((15 - tSOY)/15)*(180/hailMath::pi);
				tAngles[6] = -2 * tAngles[5];
				tAngles[7] = tAngles[1];
				tAngles[8] = tAngles[1];
				tAngles[9] = tAngles[5];
				tAngles[10] = tAngles[6];
				stepIn = false;
				stepOut = false;
				break;
			}
		}
		// Baby holding
		if (hasBaby) {
			tAngles[3] = 52.5;
			tAngles[4] = -65;
			tAngles[7] = 52.5;
			tAngles[8] = -65;
		}
		// Smoothing
		for (int i = 0; i < 11; i++) {
			angles[i] += tAngles[i] * 0.1;
			angles[i] /= 1.1; //*/ angles[i] = tAngles[i];
		}
		sOY += tSOY * 0.1;
		sOY /= 1.1;
		sOX += tSOX * 0.1;
		sOX /= 1.1;
	}

	void Player::setImage(int relevantImg, SDLwrapper::Image * setTo) {
		switch (relevantImg) {
			case 0: {
				babyImg = setTo;
				break;
			}
		}
	}

	void Player::draw(SDLwrapper::Window * window) {
		flipMult = getFlipMult();
		mX = -2.5;
		mY = 10;
		cX = 0;
		cY = 0;
		tX = 0;
		tY = -9;
		bX = cos(angles[0] * (hailMath::pi / 180));
		bY = sin(angles[0] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		// Chest calculated!
		tX = 0;
		tY = 5;
		cX += aX;
		cY += aY;
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		tX = aX;
		tY = aY;
		bX = cos(angles[0] * (hailMath::pi / 180));
		bY = sin(angles[0] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		tX = aX;
		tY = aY;
		bX = cos(angles[7] * (hailMath::pi / 180));
		bY = sin(angles[7] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		window->drawImageEx(sprites[3], collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, (angles[0] + angles[7]) * flipMult);
		tX = 0;
		tY = 5;
		cX += aX * 2;
		cY += aY * 2;
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		tX = aX;
		tY = aY;
		bX = cos(angles[8] * (hailMath::pi / 180));
		bY = sin(angles[8] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		window->drawImageEx(hasBaby?sprites[7]:sprites[4], collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, (angles[0] + angles[7] + angles[8]) * flipMult);
		mX = -2.5;
		mY = 9;
		cX = 0;
		cY = 0;
		tX = 0;
		tY = 5;
		bX = cos(angles[9] * (hailMath::pi / 180));
		bY = sin(angles[9] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		window->drawImageEx(sprites[5], collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, angles[9] * flipMult);
		tX = 0;
		tY = 5;
		cX += aX * 2;
		cY += aY * 2;
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		tX = aX;
		tY = aY;
		bX = cos(angles[10] * (hailMath::pi / 180));
		bY = sin(angles[10] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		window->drawImageEx(stepIn?sprites[8]:sprites[6], collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, (angles[9] + angles[10]) * flipMult);
		mX = -5;
		mY = 10;
		cX = 0;
		cY = 0;
		tX = 0;
		tY = -7.5;
		bX = cos(angles[0] * (hailMath::pi / 180));
		bY = sin(angles[0] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		window->drawImageEx(sprites[0], collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 10, 15.0, flipped, false, angles[0] * flipMult);
		cX += aX * 2;
		cY += aY * 2;
		if (hasBaby) {
			mX = -3.5;
			mY = 10;
			tX = -8.5;
			tY = -6.5;
			bX = cos(angles[0] * (hailMath::pi / 180));
			bY = sin(angles[0] * (hailMath::pi / 180));
			aX = tX * bX - tY * bY;
			aY = tX * bY + tY * bX;
			window->drawImageEx(babyImg, collision->getMid_x() + mX + (aX + sOX) * flipMult, collision->getBound_t() + mY + (aY + sOY), 7, 13.0, flipped, false, angles[0] * flipMult);
		}
		mX = -5;
		mY = 10;
		tX = 0;
		tY = -2;
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		tX = aX;
		tY = aY;
		bX = cos(angles[1] * (hailMath::pi / 180));
		bY = sin(angles[1] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		window->drawImageEx(sprites[1], collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 10, 10.0, flipped, false, (angles[0] + angles[1]) * flipMult);
		mX = -2.5;
		mY = 9;
		cX = 0;
		cY = 0;
		tX = 0;
		tY = 5;
		bX = cos(angles[5] * (hailMath::pi / 180));
		bY = sin(angles[5] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		window->drawImageEx(sprites[5], collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, angles[5] * flipMult);
		tX = 0;
		tY = 5;
		cX += aX * 2;
		cY += aY * 2;
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		tX = aX;
		tY = aY;
		bX = cos(angles[6] * (hailMath::pi / 180));
		bY = sin(angles[6] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		window->drawImageEx(stepOut?sprites[8]:sprites[6], collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, (angles[6] + angles[5]) * flipMult);mX = -2.5;
		mY = 10;
		cX = 0;
		cY = 0;
		tX = 0;
		tY = -9;
		bX = cos(angles[0] * (hailMath::pi / 180));
		bY = sin(angles[0] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		// Chest calculated!
		tX = 0;
		tY = 5;
		cX += aX;
		cY += aY;
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		tX = aX;
		tY = aY;
		bX = cos(angles[3] * (hailMath::pi / 180));
		bY = sin(angles[3] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		window->drawImageEx(sprites[3], collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, (angles[0] + angles[3]) * flipMult);
		tX = 0;
		tY = 5;
		cX += aX * 2;
		cY += aY * 2;
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		tX = aX;
		tY = aY;
		bX = cos(angles[0] * (hailMath::pi / 180));
		bY = sin(angles[0] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		tX = aX;
		tY = aY;
		bX = cos(angles[4] * (hailMath::pi / 180));
		bY = sin(angles[4] * (hailMath::pi / 180));
		aX = tX * bX - tY * bY;
		aY = tX * bY + tY * bX;
		window->drawImageEx(hasBaby?sprites[7]:sprites[4], collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, (angles[0] + angles[4] + angles[3]) * flipMult);
	}
}