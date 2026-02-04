#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <iostream>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "../Data/JokerData.hpp"
#include "../Objects/Card.hpp"
#include "../Effects/JokerEffects.hpp"
#include "ResourceManager.hpp"

using json = nlohmann::json;

/**
 * 游戏数据库 (GameDatabase)
 * * 单例模式，负责加载和管理所有的静态游戏数据（Joker配置、牌型筹码等）。
 * * 同时也充当对象工厂，负责创建配置好的 Joker 卡牌实例。
 */
class GameDatabase {
public:
    /**
     * 获取单例实例
     * * @return GameDatabase 的唯一引用
     */
    static GameDatabase& Instance() {
        static GameDatabase instance;
        return instance;
    }

    /**
     * 加载 Joker 数据配置
     * * 从 JSON 文件读取 Joker 的属性（名称、花费、效果ID、参数等）。
     * * @param filepath JSON 文件路径
     */
    void loadJokers(const std::string& filepath) {
        std::ifstream f(filepath);
        if (!f.is_open()) {
            std::cerr << "Failed to open joker database: " << filepath << std::endl;
            return;
        }

        json j;
        try {
            f >> j;
        } catch (const json::parse_error& e) {
            std::cerr << "JSON Parse Error (Jokers): " << e.what() << std::endl;
            return;
        }

        for (auto& [key, value] : j.items()) {
            JokerData data;
            data.id = key;
            data.name = value.value("name", "Unknown");
            data.text = value.value("text", "");
            data.cost = value.value("cost", 0);
            data.atlasIndex = value.value("atlas_id", 0);
            data.effectId = value.value("effect_id", "");
            data.params = value["params"];

            m_jokerDb[key] = data;
        }
        std::cout << "Loaded " << m_jokerDb.size() << " jokers." << std::endl;
    }

    /**
     * 创建 Joker 实例
     * * 根据 ID 查找配置，创建 Card 对象，并绑定对应的效果 (IEffect)。
     * * @param jokerId Joker 的唯一标识符 (例如 "j_greedy_joker")
     * * @return 创建好的 Card 智能指针，如果 ID 不存在返回 nullptr
     */
    std::shared_ptr<Card> createJoker(const std::string& jokerId) {
        if (m_jokerDb.find(jokerId) == m_jokerDb.end()) {
            std::cerr << "Error: Joker ID not found: " << jokerId << std::endl;
            return nullptr;
        }

        // 获取全局纹理资源
        sf::Texture& texture = ResourceManager::Instance().getTexture("jokers");

        const JokerData& data = m_jokerDb[jokerId];
        
        // 创建卡牌对象
        auto card = std::make_shared<Card>(data.atlasIndex, texture);
        
        card->setAbilityName(data.name);
        card->setCost(data.cost);
        
        // 拼接描述文本 (含价格)
        std::string descWithPrice = data.text + "\nPrice: $" + std::to_string(data.cost);
        card->setDescription(descWithPrice);
        card->setBaseScale(2.0f);

        // --- 效果绑定工厂逻辑 ---
        
        // 1. 简单倍率效果 (SimpleMult)
        if (data.effectId == "SimpleMult") {
            int amount = data.params.value("amount", 0);
            card->setEffect(std::make_shared<SimpleMultEffect>(amount));
        }
        // 2. 花色倍率效果 (SuitMult)
        else if (data.effectId == "SuitMult") {
            int amount = data.params.value("amount", 0);
            std::string suitStr = data.params.value("suit", "Spades");
            
            Suit s = Suit::Spades;
            if (suitStr == "Hearts") s = Suit::Hearts;
            else if (suitStr == "Clubs") s = Suit::Clubs;
            else if (suitStr == "Diamonds") s = Suit::Diamonds;
            
            card->setEffect(std::make_shared<SuitMultEffect>(amount, s, suitStr));
        }
        // 3. 抽象小丑效果 (AbstractJoker)
        else if (data.effectId == "AbstractJoker") {
            int amount = data.params.value("amount", 0);
            card->setEffect(std::make_shared<AbstractJokerEffect>(amount));
        }
        // 4. [新增] 弃牌返利效果 (DiscardRebate)
        // 对应第四步要求：支持弃牌触发的金钱奖励
        else if (data.effectId == "DiscardRebate") {
            int amount = data.params.value("amount", 0);
            std::string rankStr = data.params.value("rank", "Ace");
            
            Rank r = stringToRank(rankStr);
            card->setEffect(std::make_shared<DiscardRebateEffect>(amount, r));
        }

        return card;
    }

    /**
     * 加载点数筹码配置
     * * @param filepath JSON 文件路径
     */
    void loadRanks(const std::string& filepath) {
        std::ifstream f(filepath);
        if (!f.is_open()) {
            std::cerr << "Failed to open ranks database: " << filepath << std::endl;
            return;
        }

        json j;
        try {
            f >> j;
        } catch (const json::parse_error& e) {
            std::cerr << "JSON Parse Error (Ranks): " << e.what() << std::endl;
            return;
        }

        // 建立字符串到 Rank 枚举的映射 (局部使用)
        std::unordered_map<std::string, Rank> strToRank = {
            {"2", Rank::Two}, {"3", Rank::Three}, {"4", Rank::Four},
            {"5", Rank::Five}, {"6", Rank::Six}, {"7", Rank::Seven},
            {"8", Rank::Eight}, {"9", Rank::Nine}, {"10", Rank::Ten},
            {"Jack", Rank::Jack}, {"Queen", Rank::Queen}, 
            {"King", Rank::King}, {"Ace", Rank::Ace}
        };

        int loadedCount = 0;
        for (auto& [key, value] : j.items()) {
            if (strToRank.find(key) != strToRank.end()) {
                Rank r = strToRank[key];
                int chips = value.value("chips", 0);
                m_rankChips[static_cast<int>(r)] = chips;
                loadedCount++;
            }
        }
        std::cout << "Loaded chip values for " << loadedCount << " ranks from JSON." << std::endl;
    }

    /**
     * 获取指定点数的基础筹码
     * * @param rank 扑克点数
     * * @return 对应的基础筹码值
     */
    int getRankChips(Rank rank) {
        int r = static_cast<int>(rank);
        auto it = m_rankChips.find(r);
        if (it != m_rankChips.end()) {
            return it->second;
        }
        std::cerr << "[Warning] No chip value found for Rank index: " << r << " (Check ranks.json)" << std::endl;
        return 0; 
    }

    /**
     * 获取所有已加载的 Joker ID 列表
     * * 用于商店随机进货，实现数据驱动
     */
    std::vector<std::string> getAllJokerIds() const {
        std::vector<std::string> ids;
        ids.reserve(m_jokerDb.size()); // 预分配内存优化性能
        
        for (const auto& [key, val] : m_jokerDb) {
            ids.push_back(key);
        }
        return ids;
    }


private:
    GameDatabase() = default;

    /**
     * 辅助函数：字符串转 Rank 枚举
     * * 用于解析 JSON 中的 rank 参数
     */
    Rank stringToRank(const std::string& str) {
        if (str == "2") return Rank::Two;
        if (str == "3") return Rank::Three;
        if (str == "4") return Rank::Four;
        if (str == "5") return Rank::Five;
        if (str == "6") return Rank::Six;
        if (str == "7") return Rank::Seven;
        if (str == "8") return Rank::Eight;
        if (str == "9") return Rank::Nine;
        if (str == "10") return Rank::Ten;
        if (str == "Jack") return Rank::Jack;
        if (str == "Queen") return Rank::Queen;
        if (str == "King") return Rank::King;
        return Rank::Ace; // 默认为 Ace
    }
    
    std::unordered_map<std::string, JokerData> m_jokerDb;
    std::map<int, int> m_rankChips; 
};