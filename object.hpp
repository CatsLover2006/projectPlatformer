namespace ObjectHandler {
	class Object {
	public:
		CollisionHandler::Collider collision;
		virtual void draw(SDLwrapper::Window * window);
		virtual void step(double deltaTime);
		Object(CollisionHandler::Collider collision);
	};
	class DynamicObject: public Object {
	public:
		double vX, vY;
		void unintersectY(Object other);
		void unintersectX(Object other);
		virtual void draw(SDLwrapper::Window * window);
		virtual void step(double deltaTime);
		DynamicObject(CollisionHandler::Collider collision, double grav, std::vector<SDLwrapper::Image> sprites);
	private:
		double grav;
		std::vector<SDLwrapper::Image> sprites;
	};
	class GroundObject: public Object {
	public:
		long spr;
		double animSpeed;
		void draw(SDLwrapper::Window * window);
		void step(double deltaTime);
		GroundObject(CollisionHandler::Collider collision, std::vector<SDLwrapper::Image> sprites, double animSpeed);
	private:
		double animTmr;
		std::vector<SDLwrapper::Image> sprites;
	};
}