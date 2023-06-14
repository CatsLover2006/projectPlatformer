#include <string> // Error fixing
#include <iostream>
#include <vector>
#include <typeinfo>
#include <cmath>
#include "collider.hpp"
#include "hailLib/basemath.hpp"
#include "hailLib/sdlWrapper.hpp"
#include "object.hpp"
#include "player.hpp"
#include "orc.hpp"
#define ACCEL 1000
#define FRICTION 2000
#define WALK_SPEED 100
#define RUN_SPEED 300
#define GRAVITY 800

namespace GameObjects {
	bool Orc::checkAnimNoset() {
		if (anim == 3) return true;
		return false;
	}

	Orc::Orc (double x, double y, std::vector<SDLwrapper::Image *>* sprites, long positions[9], Player * player, SDLwrapper::Image * debugImage): ObjectHandler::DynamicObject(new CollisionHandler::BoxCollider(x, y, 24, 32), GRAVITY) {
		this->player = player;
		orcSprites = sprites;
		for (int i = 0; i < 8; i++) this->positions[i] = positions[i];
		this->debugImage = debugImage;
		for (int i = 0; i < 11; i++) {
			angles[i] = 0;
			tAngles[i] = 0;
		}
		tSOY = 0;
		tSOX = 0;
		sOY = 0;
		sOX = 0;
		animProgress = 0;
		type = 0;
	}

	FastOrc::FastOrc (double x, double y, std::vector<SDLwrapper::Image *>* sprites, long positions[9], Player * player, SDLwrapper::Image * debugImage): Orc(x, y, sprites, positions, player, debugImage) {}

	SmartOrc::SmartOrc (double x, double y, std::vector<SDLwrapper::Image *>* sprites, long positions[9], Player * player, SDLwrapper::Image * debugImage): Orc(x, y, sprites, positions, player, debugImage) {
		ignorePlayer = 0;
		followTime = 0;
	}

	void FastOrc::behavior(double deltaTime) {
		t_disabled = false;
		if (canSeePlayer()) {
			anim = 1;
			t_disabled = true;
			vX -= deltaTime * (ACCEL/2) * hailMath::sign<double>(collision->getMid_x() - player->collision->getMid_x());
			if (flipped && vX > 0 || !flipped && vX < 0) vX -= deltaTime * FRICTION * hailMath::sign<double>(vX);
			vX = hailMath::constrain<double>(vX, -RUN_SPEED * 1.4, RUN_SPEED * 1.4);
			flipped = (collision->getMid_x() - player->collision->getMid_x()) > 0;
		} else {
			anim = 0;
			if (hailMath::abs_q(vX) != 0) vX -= deltaTime * FRICTION * hailMath::sign<double>(vX);
		}
	}

	bool Orc::atLedge() {
		collision->x += 32 * (flipped?-1:1);
		collision->y += 32;
		for (ObjectHandler::Object * obj : objectList) {
			if (obj == this) continue;
			if (obj->isTrigger) continue;
			if (dynamic_cast<Orc*>(obj)) continue; 
			if (collision->colliding(obj->collision)) {
				collision->x -= 32 * (flipped?-1:1);
				collision->y -= 32;
				return false;
			}
		}
		collision->x -= 32 * (flipped?-1:1);
		collision->y -= 32;
		return true;
	}

	bool Orc::canSeePlayer() {
		if (hailMath::abs_q(player->collision->getMid_x() - collision->getMid_x()) <= 640) {
			for (ObjectHandler::Object * obj : objectList) {
				if (dynamic_cast<DynamicObject*>(obj)) continue;
				if (obj->isTrigger) continue;
				if (obj->collision->lineColliding(collision->getMid_x(), collision->getBound_t(), player->collision->getMid_x(), player->collision->getBound_t()) &&
					obj->collision->lineColliding(collision->getMid_x(), collision->getBound_t(), player->collision->getMid_x(), player->collision->getBound_b())) {
					return false;
				}
			}
			return true;
		} else return false;
	}

	SDLwrapper::Image * Orc::getImageAtPos(long in) {
		if (in == -1) return debugImage;
		else if (in >= orcSprites->size()) return debugImage;
		else return orcSprites->at(in);
	}

	void Orc::behavior(double deltaTime) {
		t_disabled = false;
		if (canSeePlayer()) {
			anim = 1;
			t_disabled = true;
			vX -= deltaTime * ACCEL * hailMath::sign<double>(collision->getMid_x() - player->collision->getMid_x());
			if (flipped && vX > 0 || !flipped && vX < 0) vX -= deltaTime * FRICTION * hailMath::sign<double>(vX);
			vX = hailMath::constrain<double>(vX, -RUN_SPEED, RUN_SPEED);
			flipped = (collision->getMid_x() - player->collision->getMid_x()) > 0;
		} else {
			anim = 0;
			if (hailMath::abs_q(vX) != 0) vX -= deltaTime * FRICTION * hailMath::sign<double>(vX);
		}
	}

	void SmartOrc::behavior(double deltaTime) {
		if (followTime > 0) followTime -= deltaTime;
		bool seesPlayer = canSeePlayer();
		if (seesPlayer && !ignorePlayer) {
			followTime = 0.5;
			lastSeenDir = hailMath::sign<double>(collision->getMid_x() - player->collision->getMid_x());
		}
		if (followTime <= 0) ignorePlayer = false;
		t_disabled = false;
		if (followTime > 0) {
			flipped = lastSeenDir > 0;
			if (atLedge()) {
				if (hailMath::abs_q(vX) != 0) vX -= deltaTime * FRICTION * hailMath::sign<double>(vX);
				anim = 0;
				return;
			}
			anim = 1;
			t_disabled = true;
			vX -= deltaTime * ACCEL * lastSeenDir;
			if (flipped && vX > 0 || !flipped && vX < 0) vX -= deltaTime * FRICTION * hailMath::sign<double>(vX);
			vX = hailMath::constrain<double>(vX, -RUN_SPEED/1.1, RUN_SPEED/1.1);
		} else {
			anim = 0;
			if (hailMath::abs_q(vX) != 0) vX -= deltaTime * FRICTION * hailMath::sign<double>(vX);
		}
	}

	void Orc::step(double deltaTime) {
		if (isnan(animProgress)) animProgress = 0;
		animProgress += deltaTime;
		vY += grav * deltaTime;
		behavior(deltaTime);
		if (hailMath::abs_q(vX) < 20 && !t_disabled) vX = 0; // 20 pixels per second is nothing
		collision->y -= 4;
		dontReset = false;
		bool handleOrcCollisions = false;
		for (ObjectHandler::Object * obj : objectList) {
			if (obj == this) {
				handleOrcCollisions = true;
				continue;
			}
			if (obj->isTrigger) continue;
			if (collision->colliding(obj->collision)) {
				if (obj == player) {
					continue;
				}
				if (dynamic_cast<Orc*>(obj)) {
					if (!handleOrcCollisions) continue;
					dontReset = true;
					continue;
				}
				unintersectY(obj);
				dontReset = true;
			}
		}
		if (dontReset) {
			vY = 0;
			collision->y += 1;
		}
		bool fixY = !dontReset;
		collision->x += vX * deltaTime;
		dontReset = false;
		handleOrcCollisions = false;
		for (ObjectHandler::Object * obj : objectList) {
			if (obj == this) {
				handleOrcCollisions = true;
				continue;
			}
			if (obj->isTrigger) continue;
			if (collision->colliding(obj->collision)) {
				if (obj == player) {
					player->damage(this);
					continue;
				}
				if (dynamic_cast<Orc*>(obj)) {
					if (!handleOrcCollisions) continue;
					dontReset = true;
					continue;
				}
				unintersectX(obj);
				dontReset = true;
			}
		}
		if (dontReset) {
			vX = 0;
		}
		collision->y += 4;
		collision->y += vY * deltaTime;
		if (oY < collision->y) {
			overLoad = false;
			dontReset = false;
			handleOrcCollisions = false;
			for (ObjectHandler::Object * obj : objectList) {
				if (obj == this) {
					handleOrcCollisions = true;
					continue;
				}
				if (obj->isTrigger) continue;
				if (collision->colliding(obj->collision)) {
					if (obj == player) {
						player->damage(this);
						continue;
					}
					if (dynamic_cast<Orc*>(obj)) {
					if (!handleOrcCollisions) continue;
						dontReset = true;
						continue;
					}
					unintersectY(obj);
					dontReset = true;
					if (obj->collision->getBound_t() > collision->y) overLoad = true;
				}
			}
			if (dontReset) {
				if (overLoad) vY = hailMath::min<double>(vY, 0);
				else vY = 0;
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

	int Orc::getFlipMult() {
		if (flipped) return -1;
		return 1;
	}

	void Orc::animate(short anim, double animProgress) {
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
			case 3: { // Walk
				progress = fmod(animProgress / 2, .5);
				tSOY = hailMath::abs_q(fmod(progress + .125, .25)-.125) * 3 - 3;
				tAngles[0] = 10;
				tAngles[1] = -10;
				tAngles[3] = -tAngles[0] - hailMath::abs_q(fmod(progress + .25, .5)-.25) * 100 * 4 + 40;
				tAngles[4] = -hailMath::abs_q(fmod(progress + .25, .5)-.25) * 100 * 4 + 20;
				tAngles[5] = 40 * cos(progress * hailMath::pi * 4) - 10;
				tAngles[6] = 70 + 70 * sin(progress * hailMath::pi * 4);
				tAngles[7] = -tAngles[0] + hailMath::abs_q(fmod(.75 - progress, .5)-.25) * 100 * 4 - 50;
				tAngles[8] = -hailMath::abs_q(fmod(progress, .5)-.25) * 150 * 4 + 20;
				tAngles[9] = - 40 * cos(progress * hailMath::pi * 4) - 10;
				tAngles[10] = 70 + 70 * sin(-progress * hailMath::pi * 4);
				stepIn = progress > 0.15 && progress < 0.2;
				stepOut = progress > 0.4 && progress < 0.45;
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
				progress = fmod(animProgress / 1.4, .5);
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
	}

	void FastOrc::animate(short anim, double animProgress) {
		switch (anim) {
			case 1: { // Running
				progress = fmod(animProgress / 1.2, .5);
				tSOY = hailMath::abs_q(fmod(progress + .125, .25)-.125) * 30 - 5;
				tAngles[0] = 50;
				tAngles[1] = -50;
				tAngles[3] = -tAngles[0] - hailMath::abs_q(fmod(progress + .25, .5)-.25) * 100 * 4 + 80;
				tAngles[4] = -hailMath::abs_q(fmod(progress + .25, .5)-.25) * 150 * 4 + 20;
				tAngles[5] = 80 * cos(progress * hailMath::pi * 4) + 10;
				tAngles[6] = 70 + 70 * sin(progress * hailMath::pi * 4);
				tAngles[7] = -tAngles[0] + hailMath::abs_q(fmod(.75 - progress, .5)-.25) * 100 * 4 - 40;
				tAngles[8] = -hailMath::abs_q(fmod(progress, .5)-.25) * 150 * 4 + 20;
				tAngles[9] = 30 - 80 * cos(progress * hailMath::pi * 4);
				tAngles[10] = 70 + 70 * sin(-progress * hailMath::pi * 4);
				stepIn = progress > 0.15 && progress < 0.2;
				stepOut = progress > 0.4 && progress < 0.45;
				break;
			}
			default: { // IDK what this is, backup to parent
				Orc::animate(anim, animProgress);
				break;
			}
		}
	}

	void Orc::updateLimbPos() {
		// Normal animation
		animate(anim, animProgress);
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

	void Orc::draw(SDLwrapper::Window * window) {
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
		window->drawImageEx(getImageAtPos(positions[3]), collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, (angles[0] + angles[7]) * flipMult);
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
		window->drawImageEx(getImageAtPos(positions[4]), collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, (angles[0] + angles[7] + angles[8]) * flipMult);
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
		window->drawImageEx(getImageAtPos(positions[5]), collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, angles[9] * flipMult);
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
		window->drawImageEx(getImageAtPos(positions[stepIn?8:6]), collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, (angles[9] + angles[10]) * flipMult);
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
		window->drawImageEx(getImageAtPos(positions[0]), collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 10, 19.0, flipped, false, angles[0] * flipMult);
		cX += aX * 2;
		cY += aY * 2;
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
		window->drawImageEx(getImageAtPos(positions[1]), collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 10, 10.0, flipped, false, (angles[0] + angles[1]) * flipMult);
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
		window->drawImageEx(getImageAtPos(positions[5]), collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, angles[5] * flipMult);
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
		window->drawImageEx(getImageAtPos(positions[stepOut?8:6]), collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, (angles[6] + angles[5]) * flipMult);mX = -2.5;
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
		window->drawImageEx(getImageAtPos(positions[3]), collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, (angles[0] + angles[3]) * flipMult);
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
		window->drawImageEx(getImageAtPos(positions[4]), collision->getMid_x() + mX + (aX + cX + sOX) * flipMult, collision->getBound_t() + mY + (aY + cY + sOY), 5.0, 10, flipped, false, (angles[0] + angles[4] + angles[3]) * flipMult);
	}
}