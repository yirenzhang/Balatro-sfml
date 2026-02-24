#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <vector>

#include "../Data/JokerData.hpp"
#include "../Objects/Card.hpp"

class ResourceManager;

/**
 * 游戏数据库 (GameDatabase)
 * * 单例模式，负责加载和管理所有的静态游戏数据（Joker配置、牌型筹码等）。
 * * 同时也充当对象工厂，负责创建配置好的 Joker 卡牌实例。
 */
class GameDatabase {
public:
    GameDatabase();

    void loadJokers(const std::string& filepath);
    void setResourceManager(ResourceManager* resourceManager);

    std::shared_ptr<Card> createJoker(const std::string& jokerId);

    void loadRanks(const std::string& filepath);

    int getRankChips(Rank rank);

    std::vector<std::string> getAllJokerIds() const;


private:
    std::unordered_map<std::string, JokerData> m_jokerDb;
    std::map<int, int> m_rankChips; 
    ResourceManager* m_resourceManager = nullptr;
};
