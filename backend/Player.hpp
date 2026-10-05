#pragma once
#include <string>
#include <vector>
#include <SDL2/SDL.h>
namespace music{
    struct Player{
        static SDL_AudioDeviceID device;
        bool static AddToQueue(std::string songId);
        bool static SetQueue(std::vector<std::string> songList, int startPosition);
        bool static EraseQueue();
        bool static RemoveFromQueue(std::string songId);
        bool static PauseSong();
        bool static ResumeSong();
        void static PlaySong();
        bool static SkipSong(bool direction);
        bool static DecrimentSong(int amount);
        bool static IncrementSong(int amount);
        bool static GoToTimeStampSong(double timestamp);
    };
}