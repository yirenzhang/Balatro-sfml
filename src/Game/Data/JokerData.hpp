#pragma once
#include <string>
#include <nlohmann/json.hpp> // 确保你的外部库里有这个

using json = nlohmann::json;

struct JokerData {
    std::string id;          // json key, e.g., "j_greedy_joker"
    std::string name;        // Display name
    std::string text;        // Description
    int cost = 0;
    int atlasIndex = 0;      // Index in the texture atlas
    
    std::string effectId;    // C++ class identifier (e.g., "SuitMult")
    json params;             // Raw params to pass to the effect constructor
};