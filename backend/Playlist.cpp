#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include "Playlist.hpp"
#include "../include/nlohmann/json.hpp"
namespace music{
    std::string Playlist::CreatePlaylist(std::string name){
        std::string playlistPath = "../.env/playlists.json";
        try{
            std::ifstream startFile(playlistPath);
            nlohmann::json playlists = nlohmann::json::parse(startFile);
            std::unordered_map<std::string,std::vector<int>> map;
            for(const auto& playlist : playlists) map[playlist["name"].get<std::string>()] = playlist["values"].get<std::vector<int>>();
            if(map.find(name) == map.end())map[name] = {};
            else return "Their is already a playlist with the name {" + name + "}";
            nlohmann::json output = nlohmann::json::array();
            for(const auto&[name,values]:map) output.push_back({{"name",name},{"values",values}});
            std::ofstream endFile(playlistPath);
            endFile<<output.dump(4);
            return "Playlist created";
        }catch(std::string e){
            std::cerr<<e;
            return "Failed to create playlist {" + e + "}";
        }
    }
}