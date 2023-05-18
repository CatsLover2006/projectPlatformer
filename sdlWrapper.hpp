#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

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
		void drawRect(Color * color, double x, double y, double w, double h);
		void drawRect(Color * color, SDL_Rect rect);
		void strokeRect(Color * color, double x, double y, double w, double h);
		void strokeRect(Color * color, SDL_Rect rect);
		void drawLine(Color* color, double x1, double y1, double x2, double y2);
		void drawImage(Image* image, double x, double y);
		void drawImage(Image* image, double x, double y, double w, double h);
		void drawImageEx(Image* image, double x, double y, bool flipH, bool flipV);
		void drawImageEx(Image* image, double x, double y, double w, double h, bool flipH, bool flipV);
		void drawImageEx(Image* image, double x, double y, double angle);
		void drawImageEx(Image* image, double x, double y, double w, double h, double angle);
		void drawImageEx(Image* image, double x, double y, bool flipH, bool flipV, double angle);
		void drawImageEx(Image* image, double x, double y, double w, double h, bool flipH, bool flipV, double angle);
		void runInput();
		bool keyPressed(std::string key);
		void quit();
		SDL_Renderer * internalGetRenderer();
	private: 
		SDL_Renderer *renderer;
		SDL_Window *window;
	};
}