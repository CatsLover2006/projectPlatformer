#include <string> // Error fixing
#include <vector>
#include <typeinfo>
#include "collider.hpp"
#include "sdlWrapper.hpp"
#include "object.hpp"

namespace ObjectHandler {
	Object::Object (CollisionHandler::Collider collision) {
		this->collision = collision;
	}

	void Object::draw(SDLwrapper::Window * window) {} // Stub

	void Object::step(double deltaTime) {} // Stub

	void DynamicObject::draw(SDLwrapper::Window * window) {} // Stub to Compile

	void DynamicObject::step(double deltaTime) {} // Stub to Compile

	DynamicObject::DynamicObject (CollisionHandler::Collider collision, double grav, std::vector<SDLwrapper::Image> sprites): Object(collision) {
		this->grav = grav;
		this->sprites = sprites;
	}

	GroundObject::GroundObject(CollisionHandler::Collider collision, std::vector<SDLwrapper::Image> sprites): Object(collision) {
		this->sprites = sprites;
	}

	void GroundObject::step(double deltaTime) {
		animTmr += deltaTime;
		while (animTmr > animSpeed) {
			animTmr -= animSpeed;
			spr = (spr + 1) % (sprites.size() / 9);
		}
	}

	void GroundObject::draw(SDLwrapper::Window * window) {
		window.drawImage(sprites[spr * 9], collision.x, collision.y);
		window.drawImage(sprites[spr * 9 + 2], collision.x + collision.w - sprites[spr * 9 + 2].w, collision.y);
		window.drawImage(sprites[spr * 9 + 6], collision.x, collision.y + collision.h - sprites[spr * 9 + 4].h);
		window.drawImage(sprites[spr * 9 + 2], collision.x + collision.w - sprites[spr * 9 + 6].w, collision.y + collision.h - sprites[spr * 9 + 6].h);
		//for (int xL = 0; xL +=)
	}
}