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
#include "ResourceManager.hpp" // [New] 引入资源管理器

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

    // [Modified] 简化接口：不再需要传递 texture
    std::shared_ptr<Card> createJoker(const std::string& jokerId) {
        if (m_jokerDb.find(jokerId) == m_jokerDb.end()) {
            std::cerr << "Error: Joker ID not found: " << jokerId << std::endl;
            return nullptr;
        }

        // 内部直接获取资源
        sf::Texture& texture = ResourceManager::Instance().getTexture("jokers");

        const JokerData& data = m_jokerDb[jokerId];
        // 使用获取到的纹理创建 Card
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
            return;
        }

        json j;
        try {
            f >> j;
        } catch (const json::parse_error& e) {
            std::cerr << "JSON Parse Error (Ranks): " << e.what() << std::endl;
            return;
        }

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

    int getRankChips(Rank rank) {
        int r = static_cast<int>(rank);
        auto it = m_rankChips.find(r);
        if (it != m_rankChips.end()) {
            return it->second;
        }
        std::cerr << "[Warning] No chip value found for Rank index: " << r << " (Check ranks.json)" << std::endl;
        return 0; 
    }

private:
    GameDatabase() = default;
    
    std::unordered_map<std::string, JokerData> m_jokerDb;
    std::map<int, int> m_rankChips; 
};