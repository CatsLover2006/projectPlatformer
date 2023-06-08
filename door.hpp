namespace GameObjects {
	class Door: public InteractableObject {
	public:
		void draw (SDLwrapper::Window * window);
		void step (double deltaTime);
		void handleInteraction(Player * player);
		Door (double x, double y, std::vector<SDLwrapper::Image *> * sprites, long spriteIndexes[2], SDLwrapper::Image * debugImage);
		bool backToDoor;
	private:
		std::vector<SDLwrapper::Image *> * sprites;
		double t;
		long spriteIndexes[2];
		SDLwrapper::Image * debugImage;
		SDLwrapper::Image * getImageAtPos(long in);
	};
}