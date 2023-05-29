namespace GameObjects {
	class Player: public ObjectHandler::DynamicObject {
	public:
		bool hasBaby;
		void draw (SDLwrapper::Window * window);
		void step (double deltaTime);
		void giveInput (uint8_t input);
		Player (double x, double y, std::vector<SDLwrapper::Image *> sprites);
		void setImage(int relevantImg, SDLwrapper::Image * setTo);
	private:
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
		SDLwrapper::Image * babyImg;
		short coyote;
		bool canWallJump;
		short wallDir;
		short wallCoyote;
	};
}