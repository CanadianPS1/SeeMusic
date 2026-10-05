#include <iostream>
#include <SDL2/SDL.h>
#include <fstream>
#include <thread>
extern "C"{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswresample/swresample.h>
}
#include "Player.hpp"
#include "../include/nlohmann/json.hpp"
namespace music{
    SDL_AudioDeviceID Player::device;
    void Player::PlaySong(){
        try{
            std::thread([](){
                std::ifstream envFile("../.env/env.json");
                nlohmann::json envJson = nlohmann::json::parse(envFile);
                std::ifstream queueFile("../.env/queue.json");
                nlohmann::json queueJson = nlohmann::json::parse(queueFile);
                if((queueJson["queue"].size() - 1) < queueJson["position"].get<int>()){
                    EraseQueue();
                    return;
                }
                std::string currentSongPath = queueJson["queue"].at(queueJson["position"].get<int>());
                AVFormatContext*format = nullptr;
                if(avformat_open_input(&format, currentSongPath.c_str(),nullptr,nullptr)<0) return;
                if(avformat_find_stream_info(format,nullptr) < 0) return;
                int streamIndex=av_find_best_stream(format,AVMEDIA_TYPE_AUDIO,-1,-1,nullptr,0);
                if(streamIndex < 0) return;
                AVStream*stream = format->streams[streamIndex];
                const AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
                AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
                avcodec_parameters_to_context(codecCtx,stream->codecpar);
                avcodec_open2(codecCtx,codec,nullptr);
                SwrContext*swr = swr_alloc();
                swr_alloc_set_opts2(&swr,
                    &codecCtx->ch_layout,AV_SAMPLE_FMT_S16,codecCtx->sample_rate,
                    &codecCtx->ch_layout,codecCtx->sample_fmt,codecCtx->sample_rate,
                    0,nullptr);
                swr_init(swr);
                SDL_Init(SDL_INIT_AUDIO);
                SDL_AudioSpec spec{};
                spec.freq = codecCtx->sample_rate;
                spec.format = AUDIO_S16SYS;
                spec.channels = codecCtx->ch_layout.nb_channels;
                spec.samples = 4096;
                device = SDL_OpenAudioDevice(nullptr,0,&spec,nullptr,0);
                SDL_PauseAudioDevice(device,0);
                AVPacket*packet = av_packet_alloc();
                AVFrame*frame = av_frame_alloc();
                bool skip = false;
                while(av_read_frame(format,packet) >= 0 || skip){
                    if(packet->stream_index == streamIndex){
                        avcodec_send_packet(codecCtx,packet);
                        while(avcodec_receive_frame(codecCtx,frame) >= 0 || skip){
                            uint8_t*output = nullptr;
                            int outputSamples = av_rescale_rnd(
                                swr_get_delay(swr,codecCtx->sample_rate) + frame->nb_samples,
                                codecCtx->sample_rate,
                                codecCtx->sample_rate,
                                AV_ROUND_UP
                            );
                            av_samples_alloc(
                                &output,nullptr,
                                codecCtx->ch_layout.nb_channels,
                                outputSamples,
                                AV_SAMPLE_FMT_S16,0
                            );
                            int samples=swr_convert(
                                swr,&output,outputSamples,
                                (const uint8_t**)frame->extended_data,frame->nb_samples
                            );
                            do{
                                std::ifstream envFile("../.env/env.json");
                                nlohmann::json envJson = nlohmann::json::parse(envFile);
                                bool paused = envJson["Paused"].get<bool>();
                                float volume = envJson["Volume"].get<float>();
                                for(int i=0;i<samples*codecCtx->ch_layout.nb_channels;i++)((int16_t*)output)[i] *= volume;
                                if(paused){
                                    SDL_PauseAudioDevice(device, 1);
                                    SDL_Delay(10);
                                }
                                else{
                                    SDL_PauseAudioDevice(device, 0);
                                    envFile.close();
                                    break;
                                }
                            }while(true || !skip);
                            std::ifstream envFile("../.env/env.json");
                            nlohmann::json envJson = nlohmann::json::parse(envFile);
                            if(envJson["Skip"].get<int>() == 1){
                                envJson["Skip"] = 0;
                                std::ofstream endFile("../.env/env.json");
                                endFile<<envJson.dump(4);
                                endFile.close();
                                skip = true;
                                if(envJson["Loop"] == 0 || envJson["Loop"] == 2){
                                    //queueJson["queue"].erase(queueJson["queue"].begin());
                                    if(queueJson["position"] != queueJson["queue"].size() - 1) queueJson["position"] = queueJson["position"].get<int>() + 1;
                                    else if(envJson["Loop"] == 2) queueJson["position"] = 0;
                                    else queueJson["position"] = queueJson["position"].get<int>() + 1;
                                    std::ofstream endFiles("../.env/queue.json");
                                    endFiles<<queueJson.dump(4);
                                    endFile.close();
                                }else if(envJson["Loop"] == 1){
                                    //queueJson["queue"].push_back(queueJson["queue"].at(queueJson["position"].get<int>()));
                                    //queueJson["queue"].erase(queueJson["queue"].begin());
                                    if(queueJson["position"] != queueJson["queue"].size() - 1) queueJson["position"] = queueJson["position"].get<int>() + 1;
                                    else queueJson["position"] = 0;
                                    std::ofstream endFiles("../.env/queue.json");
                                    endFiles<<queueJson.dump(4);
                                    endFile.close();
                                }
                                SDL_ClearQueuedAudio(device);
                                if(!queueJson["queue"].empty()) PlaySong();
                                return;
                            }else if(envJson["Skip"].get<int>() == -1){
                                envJson["Skip"] = 0;
                                std::ofstream endFile("../.env/env.json");
                                endFile<<envJson.dump(4);
                                endFile.close();
                                skip = true;
                                if(envJson["Loop"] == 0 || envJson["Loop"] == 2){
                                    //queueJson["queue"].erase(queueJson["queue"].begin());
                                    if(queueJson["position"] != 0) queueJson["position"] = queueJson["position"].get<int>() - 1;
                                    else if(queueJson["position"] == 0 && envJson["Loop"] == 0) queueJson["position"] = 0;
                                    else queueJson["position"] = queueJson["queue"].size() - 1;
                                    std::ofstream endFiles("../.env/queue.json");
                                    endFiles<<queueJson.dump(4);
                                    endFile.close();
                                }else if(envJson["Loop"] == 1){
                                    //queueJson["queue"].push_back(queueJson["queue"].at(queueJson["position"].get<int>()));
                                    //queueJson["queue"].erase(queueJson["queue"].begin());
                                    if(queueJson["position"] != 0) queueJson["position"] = queueJson["position"].get<int>() - 1;
                                    else queueJson["position"] = queueJson["queue"].size() - 1;
                                    std::ofstream endFiles("../.env/queue.json");
                                    endFiles<<queueJson.dump(4);
                                    endFile.close();
                                }
                                SDL_ClearQueuedAudio(device);
                                if(!queueJson["queue"].empty()) PlaySong();
                                return;
                            }
                            SDL_QueueAudio(device,output,
                                samples * codecCtx->ch_layout.nb_channels*sizeof(int16_t));
                            av_freep(&output);
                            SDL_Delay(21);
                        }
                    }
                    av_packet_unref(packet);
                }
                while(SDL_GetQueuedAudioSize(device) > 0) SDL_Delay(100);
                SDL_CloseAudioDevice(device);
                SDL_Quit();
                av_frame_free(&frame);
                av_packet_free(&packet);
                swr_free(&swr);
                avcodec_free_context(&codecCtx);
                avformat_close_input(&format);
                if(envJson["Loop"] == 0){
                    //queueJson["queue"].erase(queueJson["queue"].begin());
                    queueJson["position"] = queueJson["position"].get<int>() + 1;
                    std::ofstream endFile("../.env/queue.json");
                    endFile<<queueJson.dump(4);
                    endFile.close();
                }else if(envJson["Loop"] == 1){
                    //queueJson["queue"].push_back(queueJson["queue"].at(queueJson["position"].get<int>()));
                    //queueJson["queue"].erase(queueJson["queue"].begin());
                    if(queueJson["position"] != queueJson["queue"].size() - 1) queueJson["position"] = queueJson["position"].get<int>() + 1;
                    else queueJson["position"] = 0;
                    std::ofstream endFile("../.env/queue.json");
                    endFile<<queueJson.dump(4);
                    endFile.close();
                }
                SDL_ClearQueuedAudio(device);
                if(!queueJson["queue"].empty()) PlaySong();
                queueFile.close();
                envFile.close();
                return;
            }).detach();
        }catch(std::string e){
            std::cerr<<"Error While Playing Song {"<<e<<"}"<<std::endl;
            return;
        }
    }
    bool Player::AddToQueue(std::string songId){
        try{
            std::ifstream allSongsFile("../.env/all_songs.json");
            nlohmann::json allSongsJson = nlohmann::json::parse(allSongsFile);
            std::ifstream queueFile("../.env/queue.json");
            nlohmann::json queueJson = nlohmann::json::parse(queueFile);
            for(auto i = allSongsJson.begin(); i != allSongsJson.end(); i++){
                std::string id = i.key();
                const auto& song = i.value();
                if(id == songId){ 
                    queueJson["queue"].push_back(song["path"]);
                    std::ofstream endFile("../.env/queue.json");
                    endFile<<queueJson.dump(4);
                    endFile.close();
                }
            }
            queueFile.close();
            allSongsFile.close();
            return true;
        }catch(std::string e){
            std::cerr<<"Error adding song to queue {"<<e<<"}"<<std::endl;
            return false;
        }
    }
    bool Player::RemoveFromQueue(std::string songId){
        try{
            std::ifstream allSongsFile("../.env/all_songs.json");
            nlohmann::json allSongsJson = nlohmann::json::parse(allSongsFile);
            std::ifstream queueFile("../.env/queue.json");
            nlohmann::json queueJson = nlohmann::json::parse(queueFile);
            int index = 0;
            for(auto i = allSongsJson.begin(); i != allSongsJson.end(); i++, index++){
                std::string id = i.key();
                const auto& song = i.value();
                if(id == songId){ 
                    queueJson["queue"].erase(queueJson["queue"].begin() + index);
                    std::ofstream endFile("../.env/queue.json");
                    endFile<<queueJson.dump(4);
                    endFile.close();
                }
            }
            queueFile.close();
            allSongsFile.close();
            return true;
        }catch(std::string e){
            std::cerr<<"Error adding song to queue {"<<e<<"}"<<std::endl;
            return false;
        }
    }
    bool Player::EraseQueue(){
        try{
            std::ifstream queueFile("../.env/queue.json");
            nlohmann::json queueJson = nlohmann::json::parse(queueFile);
            queueJson = {
            {"position",0},
            {"queue",{}}
            };
            std::ofstream endFile("../.env/queue.json");
            endFile<<queueJson.dump(4);
            endFile.close();
            queueFile.close();
            return true;
        }catch(std::string e){
            std::cerr<<"Error Erasing Queue {"<<e<<"}"<<std::endl;
            return false;
        }
    }
    bool Player::PauseSong(){
        try{
            std::ifstream envFile("../.env/env.json");
            nlohmann::json envJson = nlohmann::json::parse(envFile);
            envJson["Paused"] = true;
            std::ofstream endFile("../.env/env");
            endFile<<envJson.dump(4);
            endFile.close();
            envFile.close();
            return true;
        }catch(std::string e){
            std::cerr<<"Error pausing song {"<<e<<"}"<<std::endl;
            return false;
        }
    }
    bool Player::ResumeSong(){
        try{
            std::ifstream envFile("../.env/env.json");
            nlohmann::json envJson = nlohmann::json::parse(envFile);
            envJson["Paused"] = false;
            std::ofstream endFile("../.env/env");
            endFile<<envJson.dump(4);
            endFile.close();
            envFile.close();
            return true;
        }catch(std::string e){
            std::cerr<<"Error pausing song {"<<e<<"}"<<std::endl;
            return false;
        }
    }
    bool Player::SetQueue(std::vector<std::string> songList, int startPosition){
        try{
            std::ifstream queueFile("../.env/queue.json");
            nlohmann::json queueJson = nlohmann::json::parse(queueFile);
            queueJson["position"] = startPosition;
            queueJson["queue"] = songList;
            std::ofstream endFile("../.env/queue.json");
            endFile<<queueJson.dump(4);
            endFile.close();
            queueFile.close();
            return true;
        }catch(std::string e){
            std::cerr<<"Error pausing song {"<<e<<"}"<<std::endl;
            return false;
        }
    }
    bool Player::SkipSong(bool direction){
        try{
            std::ifstream envFile("../.env/env.json");
            nlohmann::json envJson = nlohmann::json::parse(envFile);
            envJson["Skip"] = direction;
            std::ofstream endFile("../.env/env.json");
            endFile<<envJson.dump(4);
            endFile.close();
            envFile.close();
            return true;
        }catch(std::string e){
            std::cerr<<"Error skipping song {"<<e<<"}"<<std::endl;
            return false;
        }
    }
}
