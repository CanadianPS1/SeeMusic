#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <algorithm>
#include <cctype>
extern "C"{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswresample/swresample.h>
}
#include <uuid/uuid.h>
#include "Songs.hpp"
#include "Player.hpp"
#include "../include/nlohmann/json.hpp"
namespace music{
    void Song::SyncSongs(){
        try{
            std::ifstream envFile("../.env/env.json");
            nlohmann::json envJson = nlohmann::json::parse(envFile);
            std::string musicPath = envJson["MusicPath"].get<std::string>();
            std::vector<std::string> folderSongs;
            FindSongs(std::filesystem::path(std::getenv("HOME"))/musicPath, folderSongs);
            std::ifstream allSongsFile("../.env/all_songs.json");
            nlohmann::json allSongsJson = nlohmann::json::parse(allSongsFile);
            for(auto i = allSongsJson.begin(); i != allSongsJson.end();){
                if(std::find(folderSongs.begin(),folderSongs.end(),i.value()["path"].get<std::string>()) == folderSongs.end()) i = allSongsJson.erase(i);
                else i++;
            }
            for(const auto& path : folderSongs){
                bool songExists = false;
                for(const auto& song : allSongsJson) if(song["path"].get<std::string>() == path) songExists = true;
                if(songExists) continue;
                AVFormatContext*format = nullptr;
                if(avformat_open_input(&format,path.c_str(),nullptr,nullptr) < 0) continue;
                avformat_find_stream_info(format,nullptr);
                nlohmann::json songJson;
                for(unsigned int i = 0; i<format->nb_streams; i++){
                    if(format->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
                        int64_t duration = format->streams[i]->duration;
                        double seconds = duration * av_q2d(format->streams[i]->time_base);
                        songJson["duration"] = seconds;
                        break;
                    }
                }
                AVDictionaryEntry*tag = nullptr;
                std::vector<std::string> wanted = {"title","artist","album","album_artist","genre","track","date"};
                for(const auto& key:wanted){
                    AVDictionaryEntry* tag = av_dict_get(format->metadata,key.c_str(),nullptr,0);
                    if(!tag) continue;
                    std::string value = tag->value;
                    if(key == "track"){
                        size_t slash = value.find('/');
                        if(slash != std::string::npos) value = value.substr(0,slash);
                        songJson[key] = std::stoi(value);
                    }else songJson[key] = value;
                }
                songJson["path"] = path;
                allSongsJson[GetUuid()] = songJson;
            }
            std::ofstream endFile("../.env/all_songs.json");
            endFile<<allSongsJson.dump(4);
            allSongsFile.close();
            endFile.close();
            envFile.close();
        }catch(std::string e){
            std::cerr<<"Failed to Sync Songs {"<<e<<"}";
        }
    }
    void Song::FindSongs(const std::filesystem::path&dir, std::vector<std::string>&files){
        for(const auto& entry:std::filesystem::directory_iterator(dir)){
            if(entry.is_directory()){
                if(entry.path().filename()==".git") continue;
                FindSongs(entry.path(),files);
            }else if(entry.is_regular_file()){
                auto ext=entry.path().extension().string();
			    if(ext==".jpg"||ext==".jpeg"||ext==".png"||ext==".gif"||ext==".webp"||ext==".bmp"||ext==".txt"||ext==".nfo"||ext==".pdf"||ext==".ini") continue;
                files.push_back(entry.path().string());
            } 
	    }
    }
    std::string Song::GetUuid(){
        uuid_t uuid;
        char uuid_str[37];
        uuid_generate_random(uuid);
        uuid_unparse_lower(uuid, uuid_str);
        return uuid_str;
    }
    nlohmann::json Song::Search(std::string peramiterType, std::string& peramiter){
        try{
            nlohmann::json res;
            std::ifstream allSongsFile("../.env/all_songs.json");
            nlohmann::json allSongsJson = nlohmann::json::parse(allSongsFile);
            std::string search = peramiter;
            std::transform(search.begin(),search.end(),search.begin(),[](unsigned char c){return std::tolower(c);});
            for(const auto& song : allSongsJson){
                if(!song.contains(peramiterType)) continue;
                std::string value = song[peramiterType].get<std::string>();
                std::transform(value.begin(),value.end(),value.begin(),[](unsigned char c){return std::tolower(c);});
                if(value.find(search) != std::string::npos) res.push_back(song);
            }
            allSongsFile.close();
            return res;
        }catch(std::string e){
            std::cerr<<"Failed to Search {"<<e<<"}";
            return {{"error", "crached {" + e + "}"}};
        }
    }
    bool Song::FillQueueWithSearch(std::string peramiterType, std::string peramiter, int queuePosition){
        try{
            nlohmann::json songListJson = Search(peramiterType, peramiter);
            std::vector<std::string> songListVector;
            if(peramiterType == "album"){
                for (int i = 1; i < songListJson.size(); i++)
                    for(const auto& song : songListJson) if(song["track"].get<int>() == songListVector.size() + 1) songListVector.push_back(song["path"].get<std::string>());
            }else for(const auto& song : songListJson) songListVector.push_back(song["path"].get<std::string>());
            Player::SetQueue(songListVector, queuePosition);
            return true;
        }catch(std::string e){
            std::cerr<<"Failed to Fill Queue With Search {"<<e<<"}";
            return false;
        }
    }
}