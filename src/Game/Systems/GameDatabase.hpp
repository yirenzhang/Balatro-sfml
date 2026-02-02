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

using json = nlohmann::json;

class GameDatabase {
public:
    static GameDatabase& Instance() {
        static GameDatabase instance;
        return instance;
    }

    // --- 1. Joker 相关 ---
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

    std::shared_ptr<Card> createJoker(const std::string& jokerId, const sf::Texture& texture) {
        if (m_jokerDb.find(jokerId) == m_jokerDb.end()) {
            std::cerr << "Error: Joker ID not found: " << jokerId << std::endl;
            return nullptr;
        }

        const JokerData& data = m_jokerDb[jokerId];
        auto card = std::make_shared<Card>(data.atlasIndex, texture);
        card->setAbilityName(data.name);
        card->setCost(data.cost);
        std::string descWithPrice = data.text + "\nPrice: $" + std::to_string(data.cost);
        card->setDescription(descWithPrice);
        card->setBaseScale(2.0f);

        if (data.effectId == "SimpleMult") {
            int amount = data.params.value("amount", 0);
            card->setEffect(std::make_shared<SimpleMultEffect>(amount));
        }
        else if (data.effectId == "SuitMult") {
            int amount = data.params.value("amount", 0);
            std::string suitStr = data.params.value("suit", "Spades");
            Suit s = Suit::Spades;
            if (suitStr == "Hearts") s = Suit::Hearts;
            else if (suitStr == "Clubs") s = Suit::Clubs;
            else if (suitStr == "Diamonds") s = Suit::Diamonds;
            card->setEffect(std::make_shared<SuitMultEffect>(amount, s, suitStr));
        }
        else if (data.effectId == "AbstractJoker") {
            int amount = data.params.value("amount", 0);
            card->setEffect(std::make_shared<AbstractJokerEffect>(amount));
        }

        return card;
    }

    // --- 2. Rank (点数筹码) 相关 ---

    void loadRanks(const std::string& filepath) {
        std::ifstream f(filepath);
        if (!f.is_open()) {
            std::cerr << "Failed to open ranks database: " << filepath << std::endl;
            // 如果文件没找到，这里返回，m_rankChips 将为空
            // 后续 getRankChips 会返回 0，这符合“不硬编码”的要求
            return;
        }

        json j;
        try {
            f >> j;
        } catch (const json::parse_error& e) {
            std::cerr << "JSON Parse Error (Ranks): " << e.what() << std::endl;
            return;
        }

        // 字符串 -> 枚举 映射表
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
                // 你的 JSON 格式是 "2": { "chips": 2 }
                // 所以使用 value.value("chips", 0) 是正确的
                int chips = value.value("chips", 0);
                m_rankChips[static_cast<int>(r)] = chips;
                loadedCount++;
            }
        }
        std::cout << "Loaded chip values for " << loadedCount << " ranks from JSON." << std::endl;
    }

    // 查询接口
    int getRankChips(Rank rank) {
        int r = static_cast<int>(rank);
        
        // [核心修改] 恢复使用 Map 查找，完全依赖 JSON 数据
        auto it = m_rankChips.find(r);
        if (it != m_rankChips.end()) {
            return it->second;
        }

        // 如果找不到（例如 JSON 加载失败或该点数缺失），打印警告并返回 0
        // 不再进行硬编码回退
        std::cerr << "[Warning] No chip value found for Rank index: " << r << " (Check ranks.json)" << std::endl;
        return 0; 
    }

private:
    GameDatabase() = default;
    
    std::unordered_map<std::string, JokerData> m_jokerDb;
    
    // 存储点数对应的筹码值
    std::map<int, int> m_rankChips; 
};