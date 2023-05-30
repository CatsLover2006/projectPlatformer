#include <string>
#include <iostream>
#include <vector>
#include <cstdint>
#include <random>
#include <typeinfo>
#include <algorithm>
#include "collider.hpp"
#include "hailLib/sdlWrapper.hpp"
#include "object.hpp"

namespace ObjectHandler {
	std::mt19937_64 random;
	std::uniform_int_distribution<uint64_t> idGenerator(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());

	std::vector<Object*> Object::objectList;

	Object::Object (CollisionHandler::Collider * collision) {
		this->collision = collision;
		self = idGenerator(random);
		objectList.push_back(this);
		self_ptr = this;
		isTrigger = false;
	}

	void Object::draw(SDLwrapper::Window * window) {} // Stub

	void Object::step(double deltaTime) {} // Stub

	void DynamicObject::draw(SDLwrapper::Window * window) {} // Stub

	void DynamicObject::step(double deltaTime) {} // Stub

	DynamicObject::~DynamicObject() {
		for (SDLwrapper::Image * img : sprites) {
			if (img != nullptr) {
				delete img;
				img = nullptr;
			}
		}
		sprites.clear();
	}

	Object::~Object() {
		delete collision;
		objectList.erase(std::remove(objectList.begin(), objectList.end(), self_ptr), objectList.end());
		objectList.shrink_to_fit();
	}

	DynamicObject::DynamicObject (CollisionHandler::Collider * collision, double grav, std::vector<SDLwrapper::Image *> sprites): Object(collision) {
		this->grav = grav;
		this->sprites = sprites;
	}

	GroundObject::GroundObject(CollisionHandler::Collider * collision, std::vector<SDLwrapper::Image *> * sprites, long positions[9], SDLwrapper::Image * debugImage): Object(collision) {
		this->sprites = sprites;
		for (int i = 0; i < 9; i++) this->positions[i] = positions[i];
		this->debugImage = debugImage;
	}

	Slope::Slope(double x, double y, double w, double dH, std::vector<SDLwrapper::Image *> * sprites, long positions[3], SDLwrapper::Image * debugImage): Object(new CollisionHandler::RightTriCollider(x, y, x + w, y + dH, true)) {
		this->sprites = sprites;
		for (int i = 0; i < 3; i++) this->positions[i] = positions[i];
		this->debugImage = debugImage;
		this->dH = dH;
	}

	void GroundObject::step(double deltaTime) {
		// Nada
	}

	void Slope::step(double deltaTime) {
		// Nada
	}

	SDLwrapper::Image * GroundObject::getImageAtPos(long in) {
		if (in == -1) return debugImage;
		else return sprites->at(in);
	}

	SDLwrapper::Image * Slope::getImageAtPos(long in) {
		if (in == -1) return debugImage;
		else return sprites->at(in);
	}

	void GroundObject::draw(SDLwrapper::Window * window) {
		if (collision->getMid_x() == collision->x || collision->getMid_y() == collision->y) return;
		for (double x = collision->getBound_l() + getImageAtPos(positions[0])->w; x < collision->getBound_r() - getImageAtPos(positions[2])->w; x += getImageAtPos(positions[4])->w)
			for (double y = collision->getBound_t() + getImageAtPos(positions[0])->h; y < collision->getBound_b() - getImageAtPos(positions[6])->w; y += getImageAtPos(positions[4])->h)
				window->drawImage(getImageAtPos(positions[4]), x, y);
		for (double x = collision->getBound_l() + getImageAtPos(positions[0])->w; x < collision->getBound_r() - getImageAtPos(positions[2])->w; x += getImageAtPos(positions[1])->w) {
			window->drawImage(getImageAtPos(positions[1]), x, collision->getBound_t());
			window->drawImage(getImageAtPos(positions[7]), x, collision->getBound_b() - getImageAtPos(positions[7])->h);
		}
		for (double y = collision->getBound_t() + getImageAtPos(positions[0])->h; y < collision->getBound_b() - getImageAtPos(positions[5])->w; y += getImageAtPos(positions[3])->h) {
			window->drawImage(getImageAtPos(positions[3]), collision->getBound_l(), y);
			window->drawImage(getImageAtPos(positions[5]), collision->getBound_r() - getImageAtPos(positions[5])->w, y);
		}
		window->drawImage(getImageAtPos(positions[0]), collision->getBound_l(), collision->getBound_t());
		window->drawImage(getImageAtPos(positions[2]), collision->getBound_r() - getImageAtPos(positions[2])->w, collision->getBound_t());
		window->drawImage(getImageAtPos(positions[6]), collision->getBound_l(), collision->getBound_b() - getImageAtPos(positions[6])->h);
		window->drawImage(getImageAtPos(positions[8]), collision->getBound_r() - getImageAtPos(positions[8])->w, collision->getBound_b() - getImageAtPos(positions[8])->h);
	}

	void Slope::draw(SDLwrapper::Window * window) {
		double t = collision->getBound_l();
		double y;
		if (dH < 0) y = collision->getBound_b() - getImageAtPos(positions[0])->h;
		else y = collision->getBound_t();
		double k, p;
		while (t < collision->getBound_r()) {
			window->drawImage(getImageAtPos(positions[0]), t, y);
			if (y + getImageAtPos(positions[0])->h < collision->getBound_b()) window->drawImage(getImageAtPos(positions[1]), t, y + getImageAtPos(positions[0])->h);
			k = y + getImageAtPos(positions[0])->h + getImageAtPos(positions[1])->h;
			while (k < collision->getBound_b()) {
				for (p = 0; p < getImageAtPos(positions[0])->w; p += getImageAtPos(positions[2])->w) {
					window->drawImage(getImageAtPos(positions[2]), p + t, k);
				}
				k += getImageAtPos(positions[2])->h;
			}
			if (dH < 0) y -= getImageAtPos(positions[0])->h;
			else y += getImageAtPos(positions[0])->h;
			t += getImageAtPos(positions[0])->w;
		}
	}

	void DynamicObject::unintersectY(Object * other) {
		DynamicObject * otherD = dynamic_cast<DynamicObject*>(other);
		double accuracy = 1;
		while (accuracy > 0.000000001) {
			while (collision->colliding(other->collision)) {
				if (otherD) {
					if (vY < otherD->vY) {
						collision->y += accuracy;
						other->collision->y -= accuracy;
					} else {
						collision->y -= accuracy;
						other->collision->y += accuracy;
					}
				} else {
					if (vY < 0)
						collision->y += accuracy;
					else
						collision->y -= accuracy;
				}
			}
			accuracy/=10;
			while (!collision->colliding(other->collision)) {
				if (otherD) {
					if (vY < otherD->vY) {
						collision->y -= accuracy;
						other->collision->y += accuracy;
					} else {
						collision->y += accuracy;
						other->collision->y -= accuracy;
					}
				} else {
					if (vY < 0)
						collision->y -= accuracy;
					else
						collision->y += accuracy;
				}
			}
			accuracy/=10;
		}
		while (collision->colliding(other->collision)) {
			if (otherD) {
				DynamicObject * otherD = (DynamicObject *) other;
				if (vY < otherD->vY) {
					collision->y += accuracy;
					other->collision->y -= accuracy;
				} else {
					collision->y -= accuracy;
					other->collision->y += accuracy;
				}
			} else {
				if (vY < 0)
					collision->y += accuracy;
				else
					collision->y -= accuracy;
			}
		}
	}

	void DynamicObject::unintersectX(Object * other) {
		DynamicObject * otherD = dynamic_cast<DynamicObject*>(other);
		double accuracy = 1;
		while (accuracy > 0.000000001) {
			while (collision->colliding(other->collision)) {
				if (otherD) {
					if (vX < otherD->vX) {
						collision->x += accuracy;
						other->collision->x -= accuracy;
					} else {
						collision->x -= accuracy;
						other->collision->x += accuracy;
					}
				} else {
					if (vX < 0)
						collision->x += accuracy;
					else
						collision->x -= accuracy;
				}
			}
			accuracy/=10;
			while (!collision->colliding(other->collision)) {
				if (otherD) {
					if (vX < otherD->vX) {
						collision->x -= accuracy;
						other->collision->x += accuracy;
					} else {
						collision->x += accuracy;
						other->collision->x -= accuracy;
					}
				} else {
					if (vX < 0)
						collision->x -= accuracy;
					else
						collision->x += accuracy;
				}
			}
			accuracy/=10;
		}
		while (collision->colliding(other->collision)) {
			if (otherD) {
				if (vX < otherD->vX) {
					collision->x += accuracy;
					other->collision->x -= accuracy;
				} else {
					collision->x -= accuracy;
					other->collision->x += accuracy;
				}
			} else {
				if (vX < 0)
					collision->x += accuracy;
				else
					collision->x -= accuracy;
			}
		}
	}
}