namespace GameObjects {
	class Crib: public InteractableObject {
	public:
		void draw (SDLwrapper::Window * window);
		void step (double deltaTime);
		void handleInteraction(Player * player);
		SDLwrapper::Image* handleInteraction();
		Crib (double x, double y, std::vector<SDLwrapper::Image *> * sprites, long spriteIndexes[3], SDLwrapper::Image * debugImage);
	private:
		std::vector<SDLwrapper::Image *> * sprites;
		double babyAngle;
		bool reverseBabyAngle;
		bool hasBaby;
		long spriteIndexes[3];
		SDLwrapper::Image * debugImage;
		SDLwrapper::Image * getImageAtPos(long in);
	};
}