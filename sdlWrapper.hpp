namespace SDLwrapper {
	class Color {
	public:
		uint8_t r, g, b, a;
		Color(uint8_t r, uint8_t g, uint8_t b);
		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	};
	class Window {
	public:
		long width, height;

		Window(long w, long h);
		Window(long w, long h, std::string title);

		void presentWindow();
		void clearScreen(Color * color);
		void drawRect(Color * color, int x, int y, int w, int h);
		void drawRect(Color * color, SDL_Rect rect);
		void strokeRect(Color * color, int x, int y, int w, int h);
		void strokeRect(Color * color, SDL_Rect rect);
		void runInput();
	private: 
		SDL_Renderer *renderer;
		SDL_Window *window;
	};
}