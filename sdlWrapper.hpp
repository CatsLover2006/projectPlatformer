#include "SDL.h"
#include "SDL_image.h"

namespace SDLwrapper {
	class Window;
	class Color;
	class Image;
	class Window;
	class Color {
	public:
		uint8_t r, g, b, a;
		Color(uint8_t r, uint8_t g, uint8_t b);
		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	};
	class Image {
	public:
		uint64_t linkedWindow, imageID;

		SDL_Texture * getImage();
		long w, h;
		Image(std::string filename, Window * context);
		~Image();
	private:
		SDL_Texture * tex;
	};
	class Window {
	public:
		uint64_t windowID;

		long width, height;

		int mouseX, mouseY;

		std::vector<SDL_Keycode> keysDown;

		Window(long w, long h);
		Window(long w, long h, std::string title);

		void presentWindow();
		void clearScreen(Color * color);
		void drawRect(Color * color, int x, int y, int w, int h);
		void drawRect(Color * color, SDL_Rect rect);
		void strokeRect(Color * color, int x, int y, int w, int h);
		void strokeRect(Color * color, SDL_Rect rect);
		void drawLine(Color* color, int x1, int y1, int x2, int y2);
		void drawImage(Image* image, int x, int y);
		void drawImage(Image* image, int x, int y, int w, int h);
		void runInput();
		void quit();
		SDL_Renderer * internalGetRenderer();
	private: 
		SDL_Renderer *renderer;
		SDL_Window *window;
	};
}