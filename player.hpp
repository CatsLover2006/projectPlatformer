namespace GameObjects {
	class Player: public ObjectHandler::DynamicObject {
	public:
		bool hasBaby;
		void draw (SDLwrapper::Window * window);
		void step (double deltaTime);
		void giveInput (uint8_t input);
		Player (double x, double y, std::vector<SDLwrapper::Image *> sprites);
	private:
		uint8_t inputState;
		double animProgress;
		short anim;
	};
}