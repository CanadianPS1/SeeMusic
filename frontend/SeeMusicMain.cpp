#include <chrono>
#include <iostream>
#include "SeeMusicMain.hpp"
#include "RendererGl.hpp"
#include "../backend/Songs.hpp"
#include "../backend/Player.hpp"
namespace renderer{
    SeeMusic::SeeMusic(SDL_Window* window, RendererGL* rendererGL){
        if(window != nullptr && rendererGL != nullptr){
            auto time1 = std::chrono::system_clock::now();
            auto time2 = std::chrono::system_clock::now();
            bool running = true;
            music::Song::SyncSongs();
            //music::Song::FillQueueWithSearch("album", "Until the Sun Explodes", 0);
            bool songPlayed = false;
            while(running){
                time2 = std::chrono::system_clock::now();
                std::chrono::duration<float> timeDelta = time2 - time1;
                float timeDeltaFloat = timeDelta.count();
                time1 = time2;
                const float dT = std::min(timeDeltaFloat, 1.0f /20.0f);
                processEvents(running);
                draw(window, rendererGL);
                if(!songPlayed){
                    music::Player::PlaySong();
                    songPlayed = true;
                    std::cout<<"song played"<<std::endl;
                }
            }
        }
    }
    SeeMusic::~SeeMusic(){
        
    }
    void SeeMusic::processEvents(bool& running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    std::cout<<"scancode: "<<event.key.keysym.scancode<<std::endl;
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_ESCAPE:
                            running = false;
                            break;
                        case SDL_SCANCODE_A:
                            break;
                        case SDL_SCANCODE_B:
                            break;
                        case SDL_SCANCODE_C:
                            break;
                        case SDL_SCANCODE_D:
                            break;
                        case SDL_SCANCODE_E:
                            break;
                        case SDL_SCANCODE_F:
                            break;
                        case SDL_SCANCODE_G:
                            break;
                        case SDL_SCANCODE_H:
                            break;
                        case SDL_SCANCODE_I:
                            break;
                        case SDL_SCANCODE_J:
                            break;
                        case SDL_SCANCODE_K:
                            break;
                        case SDL_SCANCODE_L:
                            break;
                        case SDL_SCANCODE_M:
                            break;
                        case SDL_SCANCODE_N:
                            break;
                        case SDL_SCANCODE_O:
                            break;
                        case SDL_SCANCODE_P:
                            break;
                        case SDL_SCANCODE_Q:
                            break;
                        case SDL_SCANCODE_R:
                            break;
                        case SDL_SCANCODE_S:
                            break;
                        case SDL_SCANCODE_T:
                            break;
                        case SDL_SCANCODE_U:
                            break;
                        case SDL_SCANCODE_V:
                            break;
                        case SDL_SCANCODE_W:
                            break;
                        case SDL_SCANCODE_X:
                            break;
                        case SDL_SCANCODE_Y:
                            break;
                        case SDL_SCANCODE_Z:
                            break;
                        case SDL_SCANCODE_1:
                            break;
                        case SDL_SCANCODE_2:
                            break;
                        case SDL_SCANCODE_3:
                            break;
                        case SDL_SCANCODE_4:
                            break;
                        case SDL_SCANCODE_5:
                            break;
                        case SDL_SCANCODE_6:
                            break;
                        case SDL_SCANCODE_7:
                            break;
                        case SDL_SCANCODE_8:
                            break;
                        case SDL_SCANCODE_9:
                            break;
                        case SDL_SCANCODE_0:
                            break;
                        case SDL_SCANCODE_RETURN:
                            break;
                        case SDL_SCANCODE_BACKSPACE:
                            break;
                        case SDL_SCANCODE_TAB:
                            break;
                        case SDL_SCANCODE_SPACE:
                            break;
                        case SDL_SCANCODE_MINUS:
                            break;
                        case SDL_SCANCODE_EQUALS:
                            break;
                        case SDL_SCANCODE_LEFTBRACKET:
                            break;
                        case SDL_SCANCODE_RIGHTBRACKET:
                            break;
                        case SDL_SCANCODE_BACKSLASH:
                            break;
                        case SDL_SCANCODE_NONUSHASH:
                            break;
                        case SDL_SCANCODE_SEMICOLON:
                            break;
                        case SDL_SCANCODE_APOSTROPHE:
                            break;
                        case SDL_SCANCODE_GRAVE:
                            break;
                        case SDL_SCANCODE_COMMA:
                            break;
                        case SDL_SCANCODE_PERIOD:
                            break;
                        case SDL_SCANCODE_SLASH:
                            break;
                        case SDL_SCANCODE_CAPSLOCK:
                            break;
                        case SDL_SCANCODE_RIGHT:
                            break;
                        case SDL_SCANCODE_LEFT:
                            break;
                        case SDL_SCANCODE_DOWN:
                            break;
                        case SDL_SCANCODE_UP:
                            break;
                        case SDL_SCANCODE_AUDIONEXT:
                            music::Player::SkipSong(1);
                            std::cout<<"skip forword pressed"<<std::endl;
                            break;
                        case SDL_SCANCODE_AUDIOPREV:
                            music::Player::SkipSong(-1);
                            std::cout<<"skip backword pressed"<<std::endl;
                            break;
                        case SDL_SCANCODE_AUDIOSTOP:
                            music::Player::PauseSong();
                            std::cout<<"pause pressed"<<std::endl;
                            break;
                        case SDL_SCANCODE_AUDIOPLAY:
                            music::Player::PlaySong();
                            std::cout<<"play pressed"<<std::endl;
                            break;
                        case SDL_SCANCODE_MEDIASELECT:
                            break;
                        case SDL_SCANCODE_AUDIOREWIND:
                            break;
                        case SDL_SCANCODE_AUDIOFASTFORWARD:
                            break;
                        }
            }
        }
    }
    void SeeMusic::draw(SDL_Window* window, RendererGL* rendererGL){
        if(rendererGL == nullptr) return;
        glClearColor(0.0f, 0.25f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_Rect rect = {50,80,150,80};
        rendererGL->setDrawColor(213,116,0,255);
        rendererGL->fillOval(&rect);
        rect = {110,260,110,130};
        rendererGL->setDrawColor(255,255,0,255);
        rendererGL->fillOval(&rect);
        rect = {320,125,150,200};
        rendererGL->setDrawColor(190,74,194,255);
        rendererGL->fillOval(&rect);
        rect = {610,60,150,180};
        rendererGL->setDrawColor(0,224,74,255);
        rendererGL->fillOval(&rect);
        rect = {590,330,240,135};
        rendererGL->setDrawColor(255,0,0,255);
        rendererGL->fillOval(&rect);
        rendererGL->swapWindow();
    }
}