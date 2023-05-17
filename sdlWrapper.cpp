#include <string>
#include <cstdint>
#include <random>
#include <vector>
#include "sdlWrapper.hpp"

namespace SDLwrapper {
	std::mt19937_64 random;
	std::uniform_int_distribution<uint64_t> idGenerator(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
	SDL_Surface * texLoad;
	Color::Color(uint8_t r, uint8_t g, uint8_t b) {
		Color(r, g, b, 255);
	}
	Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	SDL_Texture * Image::getImage() {
		return tex;
	}

	Image::Image(std::string filename, Window * context) {
		texLoad = IMG_Load(filename.c_str());
		tex = SDL_CreateTextureFromSurface(context->internalGetRenderer(), texLoad);
		w = texLoad->w;
		h = texLoad->h;
		SDL_FreeSurface(texLoad);
		linkedWindow = context->windowID;
		imageID = idGenerator(random);
	}

	Image::~Image() {
		SDL_DestroyTexture(tex);
	}

	SDL_Renderer * Window::internalGetRenderer() {
		return renderer;
	}

	Window::Window(long w, long h) {
		Window(w, h, "SDL Window");
	}
	Window::Window(long w, long h, std::string name) {
		int rendererFlags, windowFlags;
		rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
		windowFlags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL;
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			printf("Couldn't initialize SDL: %s\n", SDL_GetError());
			exit(1);
		}
		window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, windowFlags);
		if (!window) {
			printf("Failed to open %ld x %ld window: %s\n", w, h, SDL_GetError());
			exit(1);
		}
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		renderer = SDL_CreateRenderer(window, -1, rendererFlags);
		if (!renderer) {
			printf("Failed to create renderer: %s\n", SDL_GetError());
			exit(1);
		}
		windowID = idGenerator(random);
	}

	void Window::presentWindow() {
		SDL_RenderPresent(renderer);
	}

	void Window::clearScreen(Color * color) {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
		SDL_RenderClear(renderer);
	}

	void Window::drawRect(Color * color, int x, int y, int w, int h) {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
    	SDL_RenderFillRect(renderer, &rect);
	}

	void Window::drawRect(Color * color, SDL_Rect rect) {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    	SDL_RenderFillRect(renderer, &rect);
	}

	void Window::strokeRect(Color * color, int x, int y, int w, int h) {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
    	SDL_RenderDrawRect(renderer, &rect);
	}

	void Window::strokeRect(Color * color, SDL_Rect rect) {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    	SDL_RenderDrawRect(renderer, &rect);
	}

	void Window::drawLine(Color* color, int x1, int y1, int x2, int y2) {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	}

	void Window::drawImage(Image* image, int x, int y) {
		drawImage(image, x, y, image->w, image->h);
	}

	void Window::drawImage(Image* image, int x, int y, int w, int h) {
		if (windowID != image->linkedWindow) return;
		SDL_Rect loc;
		loc.x = x;
		loc.y = y;
		loc.w = w;
		loc.h = h;
		SDL_RenderCopy(renderer, image->getImage(), NULL, &loc);
		// TODO: draw the image;
	}

	void Window::runInput() {
		SDL_GetMouseState(&mouseX, &mouseY);
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					exit(0);
					break;
				}
				case SDL_KEYDOWN:
				case SDL_KEYUP: {
					if (event.key.repeat) break;
					switch (event.key.type) {
						case SDL_KEYDOWN: {
							keysDown.push_back(event.key.keysym.sym);
							break;
						}
						default: {
							for (std::vector<SDL_Keycode>::iterator i = keysDown.begin(); i != keysDown.end();) {
								if (*i == event.key.keysym.sym) {
									keysDown.erase(i);
								}
							}
						}
					}
					break;
				}
				default:
					break;
			}
		}
	}

	void Window::quit() {
		SDL_Quit();
	}
}