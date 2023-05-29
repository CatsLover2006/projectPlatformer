// This entire file is just linker stubs

#include <typeinfo>
#include <cmath>
#include <vector>
#include "collider.hpp"
#include "hailLib/basemath.hpp"
#include "hailLib/sdlWrapper.hpp"
#include "object.hpp"
#include "playerInteractableObjects.hpp"

namespace GameObjects {
	InteractableObject::InteractableObject(CollisionHandler::Collider * collision): ObjectHandler::Object(collision) {
		isTrigger = true;
	} // Linker stubs
	void InteractableObject::draw(SDLwrapper::Window * window) {}
	void InteractableObject::step(double deltaTime) {}
	void InteractableObject::handleInteraction(Player * player) {}
}