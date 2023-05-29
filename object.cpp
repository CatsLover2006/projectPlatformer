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

	GroundObject::~GroundObject() { // Stub 4 now
	}

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

	void GroundObject::step(double deltaTime) {
		// Nada
	}

	SDLwrapper::Image * GroundObject::getImageAtPos(long in) {
		if (in == -1) return debugImage;
		else return sprites->at(in);
	}

	void GroundObject::draw(SDLwrapper::Window * window) {
		for (double x = collision->getBound_l() + getImageAtPos(positions[0])->w; x < collision->getBound_r() - getImageAtPos(positions[2])->w; x += getImageAtPos(positions[4])->w)
			for (double y = collision->getBound_t() + getImageAtPos(positions[0])->h; y < collision->getBound_b() - getImageAtPos(positions[6])->w; y += getImageAtPos(positions[4])->h)
				window->drawImage(getImageAtPos(positions[4]), x, y);
		for (double x = collision->getBound_l() + getImageAtPos(positions[0])->w; x < collision->getBound_r() - getImageAtPos(positions[2])->w; x += getImageAtPos(positions[1])->w) {
			window->drawImage(getImageAtPos(positions[1]), x, collision->getBound_t());
			window->drawImage(getImageAtPos(positions[6]), x, collision->getBound_b() - getImageAtPos(positions[6])->h);
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

	void DynamicObject::unintersectY(Object * other) {
		double accuracy = 1;
		while (accuracy > 0.000000001) {
			while (collision->colliding(other->collision)) {
				if (typeid(other) == typeid(DynamicObject)) {
					if (collision->getMid_y() > other->collision->getMid_y()) {
						collision->y += accuracy;
						other->collision->y -= accuracy;
					} else {
						collision->y -= accuracy;
						other->collision->y += accuracy;
					}
				} else {
					if (collision->getMid_y() > other->collision->getMid_y())
						collision->y += accuracy;
					else
						collision->y -= accuracy;
				}
			}
			accuracy/=10;
			while (!collision->colliding(other->collision)) {
				if (typeid(other) == typeid(DynamicObject)) {
					if (collision->getMid_y() > other->collision->getMid_y()) {
						collision->y -= accuracy;
						other->collision->y += accuracy;
					} else {
						collision->y += accuracy;
						other->collision->y -= accuracy;
					}
				} else {
					if (collision->getMid_y() > other->collision->getMid_y())
						collision->y -= accuracy;
					else
						collision->y += accuracy;
				}
			}
			accuracy/=10;
		}
		while (collision->colliding(other->collision)) {
			if (typeid(other) == typeid(DynamicObject)) {
				if (collision->getMid_y() > other->collision->getMid_y()) {
					collision->y += accuracy;
					other->collision->y -= accuracy;
				} else {
					collision->y -= accuracy;
					other->collision->y += accuracy;
				}
			} else {
				if (collision->getMid_y() > other->collision->getMid_y())
					collision->y += accuracy;
				else
					collision->y -= accuracy;
			}
		}
	}

	void DynamicObject::unintersectX(Object * other) {
		double accuracy = 1;
		while (accuracy > 0.000000001) {
			while (collision->colliding(other->collision)) {
				if (typeid(other) == typeid(DynamicObject)) {
					if (collision->getMid_x() > other->collision->getMid_x()) {
						collision->x += accuracy;
						other->collision->x -= accuracy;
					} else {
						collision->x -= accuracy;
						other->collision->x += accuracy;
					}
				} else {
					if (collision->getMid_x() > other->collision->getMid_x())
						collision->x += accuracy;
					else
						collision->x -= accuracy;
				}
			}
			accuracy/=10;
			while (!collision->colliding(other->collision)) {
				if (typeid(other) == typeid(DynamicObject)) {
					if (collision->getMid_x() > other->collision->getMid_x()) {
						collision->x -= accuracy;
						other->collision->x += accuracy;
					} else {
						collision->x += accuracy;
						other->collision->x -= accuracy;
					}
				} else {
					if (collision->getMid_x() > other->collision->getMid_x())
						collision->x -= accuracy;
					else
						collision->x += accuracy;
				}
			}
			accuracy/=10;
		}
		while (collision->colliding(other->collision)) {
			if (typeid(other) == typeid(DynamicObject)) {
				if (collision->getMid_x() > other->collision->getMid_x()) {
					collision->x += accuracy;
					other->collision->x -= accuracy;
				} else {
					collision->x -= accuracy;
					other->collision->x += accuracy;
				}
			} else {
				if (collision->getMid_x() > other->collision->getMid_x())
					collision->x += accuracy;
				else
					collision->x -= accuracy;
			}
		}
	}
}