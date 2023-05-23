#include <string> // Error fixing
#include <iostream>
#include <vector>
#include <typeinfo>
#include "collider.hpp"
#include "sdlWrapper.hpp"
#include "object.hpp"

namespace ObjectHandler {
	Object::Object (CollisionHandler::Collider * collision) {
		this->collision = collision;
	}

	void Object::draw(SDLwrapper::Window * window) {} // Stub

	void Object::step(double deltaTime) {} // Stub

	void DynamicObject::draw(SDLwrapper::Window * window) {} // Stub

	void DynamicObject::step(double deltaTime) {} // Stub

	DynamicObject::DynamicObject (CollisionHandler::Collider * collision, double grav, std::vector<SDLwrapper::Image *> sprites): Object(collision) {
		this->grav = grav;
		this->sprites = sprites;
	}

	GroundObject::GroundObject(CollisionHandler::Collider * collision, std::vector<SDLwrapper::Image *> sprites, double animSpeed): Object(collision) {
		this->sprites = sprites;
		this->animSpeed = animSpeed;
	}

	void GroundObject::step(double deltaTime) {
		animTmr += deltaTime;
		while (animTmr > animSpeed) {
			animTmr -= animSpeed;
			spr = (spr + 1) % (sprites.size() / 9);
		}
	}

	void GroundObject::draw(SDLwrapper::Window * window) {
		for (double x = collision->getBound_l() + sprites[spr * 9]->w; x < collision->getBound_r() - sprites[spr * 9 + 2]->w; x += sprites[spr * 9 + 4]->w)
			for (double y = collision->getBound_t() + sprites[spr * 9]->h; y < collision->getBound_b() - sprites[spr * 9 + 6]->w; y += sprites[spr * 9 + 4]->h)
				window->drawImage(sprites[spr * 9 + 4], x, y);
		for (double x = collision->getBound_l() + sprites[spr * 9]->w; x < collision->getBound_r() - sprites[spr * 9 + 2]->w; x += sprites[spr * 9 + 1]->w) {
			window->drawImage(sprites[spr * 9 + 1], x, collision->getBound_t());
			window->drawImage(sprites[spr * 9 + 7], x, collision->getBound_b() - sprites[spr * 9 + 7]->h);
		}
		for (double y = collision->getBound_t() + sprites[spr * 9]->h; y < collision->getBound_b() - sprites[spr * 9 + 6]->w; y += sprites[spr * 9 + 3]->h) {
			window->drawImage(sprites[spr * 9 + 3], collision->getBound_l(), y);
			window->drawImage(sprites[spr * 9 + 6], collision->getBound_r() - sprites[spr * 9 + 6]->w, y);
		}
		window->drawImage(sprites[spr * 9], collision->getBound_l(), collision->getBound_t());
		window->drawImage(sprites[spr * 9 + 2], collision->getBound_r() - sprites[spr * 9 + 2]->w, collision->getBound_t());
		window->drawImage(sprites[spr * 9 + 6], collision->getBound_l(), collision->getBound_b() - sprites[spr * 9 + 6]->h);
		window->drawImage(sprites[spr * 9 + 8], collision->getBound_r() - sprites[spr * 9 + 8]->w, collision->getBound_b() - sprites[spr * 9 + 8]->h);
	}
}