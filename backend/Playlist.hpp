#pragma once
#include <string>
#include <vector>
namespace music{
    struct Playlist{
        std::string static CreatePlaylist(std::string name);
        bool static DeletePlaylist(std::string name);
        bool static AddSongToPlaylist(int songId, std::string playlistName);
        bool static RemoveSongFromPlaylist(int songId, std::string playlistName);
        std::vector<int> static GetSongs(std::string playlistName);
    };
}