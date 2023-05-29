namespace ObjectHandler {
	class Object {
	public:
		CollisionHandler::Collider * collision;
		virtual void draw(SDLwrapper::Window * window);
		virtual void step(double deltaTime);
		Object(CollisionHandler::Collider * collision);
		virtual ~Object();
		Object * self_ptr;
		bool isTrigger;
	protected:
		static std::vector<Object*> objectList;
		uint64_t self;
	};
	class DynamicObject: public Object {
	public:
		double vX, vY;
		void unintersectY(Object * other);
		void unintersectX(Object * other);
		virtual void draw(SDLwrapper::Window * window);
		virtual void step(double deltaTime);
		~DynamicObject();
		DynamicObject(CollisionHandler::Collider * collision, double grav, std::vector<SDLwrapper::Image *> sprites);
	protected:
		double grav;
		std::vector<SDLwrapper::Image *> sprites;
	};
	class GroundObject: public Object {
	public:
		void draw(SDLwrapper::Window * window);
		void step(double deltaTime);
		GroundObject(CollisionHandler::Collider * collision, std::vector<SDLwrapper::Image *> * sprites, long positions[9], SDLwrapper::Image * debugImage);
	private:
		long positions[9];
		std::vector<SDLwrapper::Image *> * sprites;
		SDLwrapper::Image * debugImage;
		SDLwrapper::Image * getImageAtPos(long in);
	};
	class Slope: public Object {
	public:
		void draw(SDLwrapper::Window * window);
		void step(double deltaTime);
		Slope(double x, double y, double w, double dH, std::vector<SDLwrapper::Image *> * sprites, long positions[3], SDLwrapper::Image * debugImage);
	private:
		long spr;
		long positions[3];
		double dH;
		std::vector<SDLwrapper::Image *> * sprites;
		SDLwrapper::Image * debugImage;
		SDLwrapper::Image * getImageAtPos(long in);
	};
}