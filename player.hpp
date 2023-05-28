namespace GameObjects {
	class Player: public ObjectHandler::DynamicObject {
	public:
		bool hasBaby;
		void draw (SDLwrapper::Window * window);
		void step (double deltaTime);
		void giveInput (uint8_t input);
		Player (double x, double y, std::vector<SDLwrapper::Image *> sprites);
	private:
		std::vector<ObjectHandler::Object *> * collisionBoxes;
		uint8_t inputState;
		double animProgress;
		short anim;
		bool flipped;
		double angles[11]; // body, head, hair, armOut, armFout, legOut, legFout, armIn, armFin, legIn, legFin
		double tAngles[11];
		int getFlipMult();
		bool checkCanJump();
		bool checkAnimNoset();
		void updateLimbPos();
	};
}