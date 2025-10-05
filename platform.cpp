#include "platform.h"
#include <SDL3/SDL_keycode.h>
#include <iostream>

Platform::Platform(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        exit(1);
    }

    // Create window and renderer safely
    SDL_Window* win = nullptr;
    SDL_Renderer* ren = nullptr;
    if (!SDL_CreateWindowAndRenderer(title, windowWidth, windowHeight, 0, &win, &ren)) {
        std::cerr << "SDL_CreateWindowAndRenderer Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        exit(1);
    }

    if (!win || !ren) { // Extra safety check
        std::cerr << "Failed to create window or renderer\n";
        SDL_Quit();
        exit(1);
    }

    window = win;
    renderer = ren;

    /* Optional: enable VSync
    if (SDL_RenderSetVSync(renderer, true) != 0) {
        std::cerr << "Warning: Failed to set VSync: " << SDL_GetError() << "\n";
    }
    */
    // Create texture
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                textureWidth,
                                textureHeight);
    if (!texture) {
        std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << "\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
}

Platform::~Platform() {
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch) {
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                return true;

            case SDL_EVENT_KEY_DOWN: {
                SDL_Keycode key = event.key.key;
                switch (key) {
                    case SDLK_1: keys[0x1] = 1; break;
                    case SDLK_2: keys[0x2] = 1; break;
                    case SDLK_3: keys[0x3] = 1; break;
                    case SDLK_4: keys[0xC] = 1; break;

                    case SDLK_Q: keys[0x4] = 1; break;
                    case SDLK_W: keys[0x5] = 1; break;
                    case SDLK_E: keys[0x6] = 1; break;
                    case SDLK_R: keys[0xD] = 1; break;

                    case SDLK_A: keys[0x7] = 1; break;
                    case SDLK_S: keys[0x8] = 1; break;
                    case SDLK_D: keys[0x9] = 1; break;
                    case SDLK_F: keys[0xE] = 1; break;

                    case SDLK_Z: keys[0xA] = 1; break;
                    case SDLK_X: keys[0x0] = 1; break;
                    case SDLK_C: keys[0xB] = 1; break;
                    case SDLK_V: keys[0xF] = 1; break;

                    case SDLK_ESCAPE: return true; // quit on ESC
                }
                break;
            }

            case SDL_EVENT_KEY_UP: {
                SDL_Keycode key = event.key.key;
                switch (key) {
                    case SDLK_1: keys[0x1] = 0; break;
                    case SDLK_2: keys[0x2] = 0; break;
                    case SDLK_3: keys[0x3] = 0; break;
                    case SDLK_4: keys[0xC] = 0; break;

                    case SDLK_Q: keys[0x4] = 0; break;
                    case SDLK_W: keys[0x5] = 0; break;
                    case SDLK_E: keys[0x6] = 0; break;
                    case SDLK_R: keys[0xD] = 0; break;

                    case SDLK_A: keys[0x7] = 0; break;
                    case SDLK_S: keys[0x8] = 0; break;
                    case SDLK_D: keys[0x9] = 0; break;
                    case SDLK_F: keys[0xE] = 0; break;

                    case SDLK_Z: keys[0xA] = 0; break;
                    case SDLK_X: keys[0x0] = 0; break;
                    case SDLK_C: keys[0xB] = 0; break;
                    case SDLK_V: keys[0xF] = 0; break;
                }
                break;
            }
        }
    }
    return false;
}
