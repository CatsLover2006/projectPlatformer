namespace GameObjects {
	class Player;
	class InteractableObject: public ObjectHandler::Object {
	public:
		virtual void draw (SDLwrapper::Window * window);
		virtual void step (double deltaTime);
		virtual void handleInteraction(Player * player);
		InteractableObject(CollisionHandler::Collider * collision);
	};
}