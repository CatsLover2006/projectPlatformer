#include <string>
#include "SDL.h"
#include <cstdint>
#include "sdlWrapper.hpp"

namespace SDLwrapper {
	Color::Color(uint8_t r, uint8_t g, uint8_t b) {
		Color(r, g, b, 255);
	}
	Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
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

	void Window::runInput() {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					exit(0);
					break;

				default:
					break;
			}
		}
	}
}