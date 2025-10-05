#include <SDL3/SDL.h>
#include <cstdint>
#include "chip8_header.h"
#ifndef PLATFORM_H
#define PLATFORM_H
class Platform{
    public:
    Platform(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform(); // destructor cleans up resources
    void Update(void const*buffer, int pitch);
    bool ProcessInput(uint8_t* keys);

    private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
};

#endif