#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * Joker 静态配置。
 *
 * 数据库加载后保留原始参数，目的是让效果工厂拥有足够上下文，
 * 避免把玩法配置硬编码到 C++ 逻辑里。
 */
struct JokerData {
    std::string id;
    std::string name;
    std::string text;
    int cost = 0;
    int atlasIndex = 0;
    
    std::string effectId;
    json params;
};
