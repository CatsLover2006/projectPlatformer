namespace GameObjects {
	class Orc: public ObjectHandler::DynamicObject {
	public:
		void draw (SDLwrapper::Window * window);
		void step (double deltaTime);
		Orc (double x, double y, std::vector<SDLwrapper::Image *>* sprites, long positions[9], Player * player, SDLwrapper::Image * debugImage);
		void setImage(int relevantImg, SDLwrapper::Image * setTo);
		short health;
		short type;
	private:
		std::vector<SDLwrapper::Image *>* orcSprites;
		double sX, sY;
		SDLwrapper::Image * debugImage;
		SDLwrapper::Image * getImageAtPos(long in);
		double animProgress;
		short anim;
		bool flipped;
		bool canSeePlayer();
		double angles[11]; // body, head, hair, armOut, armFout, legOut, legFout, armIn, armFin, legIn, legFin
		double tAngles[11];
		int getFlipMult();
		bool checkCanJump();
		bool checkAnimNoset();
		void updateLimbPos();
		long positions[9];
		Player * player;
		double sOX, tSOX;
		double sOY, tSOY;
		double progress;
		int flipMult;
		double tX, tY, cX, cY, aX, aY, bX, bY, mX, mY;
		bool stepIn, stepOut;
		bool atLedge;
		bool dontReset;
		bool overLoad;
		bool t_disabled;
		virtual void behavior(double deltaTimes);
	};
}