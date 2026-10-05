#pragma once
#include <chrono>
#include "SDL2/SDL.h"
#include "RendererGl.hpp"
namespace renderer{
    class SeeMusic{
        public:
            SeeMusic(SDL_Window* window, RendererGL* rendererGL);
            ~SeeMusic();
        private:
            void processEvents(bool& running);
            void draw(SDL_Window* window, RendererGL* rendererGL);
    };
}