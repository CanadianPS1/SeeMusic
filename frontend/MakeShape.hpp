#pragma once
#include "SDL2/SDL.h"
#include <glm/glm.hpp>
#include "RendererGl.hpp"
namespace renderer{
    struct Shape{
        glm::vec4 color;
        SDL_Rect shape;
    };
    class CreateShape{
        public:
            static Shape* CreateRectangle(int x, int y, int w, int h, int r, int g, int b, int a);
    };
}