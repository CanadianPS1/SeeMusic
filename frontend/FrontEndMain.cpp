#include "SeeMusicMain.hpp"
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include "SDL2/SDL.h"
namespace renderer{
    class main{
        public:
            void run(){
                srand((unsigned)time(NULL));
                if(SDL_Init(SDL_INIT_VIDEO) < 0){
                    std::cout<<"ErrorL couldnt insitalize the SDL video = "<<SDL_GetError()<<std::endl;
                    return;
                }
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
                SDL_Window* window = SDL_CreateWindow("SeeMusic", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
                if(window == nullptr){
                    std::cout<<"Error: couldnt create window = "<<SDL_GetError()<<std::endl;
                    SDL_Quit();
                    return;
                }
                SDL_GLContext glContext = SDL_GL_CreateContext(window);
                if(glContext == nullptr){
                    std::cout<<"Error: Couldnt create the opengl context = "<<SDL_GetError()<<std::endl;
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return; 
                }
                try{
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    RendererGL rendererGL(window);
                    SDL_GL_SetSwapInterval(1);
                    SeeMusic seeMusic(window, &rendererGL);
                }catch(...){
                    SDL_GL_DeleteContext(glContext);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    throw;
                }
                SDL_GL_DeleteContext(glContext);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return;
            }
    };
}
