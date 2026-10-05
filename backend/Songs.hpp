#pragma once
#include <filesystem>
#include <vector>
#include "../include/nlohmann/json.hpp"
namespace music{
    struct Song{
        void static SyncSongs();
        void static FindSongs(const std::filesystem::path& dir, std::vector<std::string>& files);
        std::string static GetUuid();
        nlohmann::json static Search(std::string peramiterType, std::string& peramiter);
        bool static FillQueueWithSearch(std::string peramiterType, std::string peramiter, int queuePosition);
    };
}