#include "GameDatabase.hpp"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "JokerEffectFactory.hpp"
#include "ResourceManager.hpp"

using json = nlohmann::json;

GameDatabase::GameDatabase() = default;

void GameDatabase::recordError(const std::string& msg) {
    m_errors.push_back(msg);
    std::cerr << msg << std::endl;
}

void GameDatabase::setResourceManager(ResourceManager* resourceManager) {
    m_resourceManager = resourceManager;
}

bool GameDatabase::loadJokers(const std::string& filepath) {
    m_jokerDb.clear();

    std::ifstream f(filepath);
    if (!f.is_open()) {
        recordError("[Error] Failed to open joker database: " + filepath);
        return false;
    }

    json j;
    try {
        f >> j;
    } catch (const json::parse_error& e) {
        recordError(std::string("[Error] JSON parse error (Jokers): ") + e.what());
        return false;
    } catch (const std::exception& e) {
        recordError(std::string("[Error] Failed to read jokers json: ") + e.what());
        return false;
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
    return true;
}

std::shared_ptr<Card> GameDatabase::createJoker(const std::string& jokerId) {
    if (m_jokerDb.find(jokerId) == m_jokerDb.end()) {
        recordError("[Error] Joker ID not found: " + jokerId);
        return nullptr;
    }

    if (!m_resourceManager) {
        recordError("[Error] ResourceManager is not set for GameDatabase.");
        return nullptr;
    }

    sf::Texture& texture = m_resourceManager->getTexture("jokers");
    const JokerData& data = m_jokerDb[jokerId];

    auto card = std::make_shared<Card>(data.atlasIndex, texture);
    card->setAbilityName(data.name);
    card->setCost(data.cost);
    card->setDescription(data.text + "\nPrice: $" + std::to_string(data.cost));
    card->setBaseScale(2.0f);
    card->setEffect(JokerEffectFactory::Create(data.effectId, data.params));

    return card;
}

bool GameDatabase::loadRanks(const std::string& filepath) {
    m_rankChips.clear();

    std::ifstream f(filepath);
    if (!f.is_open()) {
        recordError("[Error] Failed to open ranks database: " + filepath);
        return false;
    }

    json j;
    try {
        f >> j;
    } catch (const json::parse_error& e) {
        recordError(std::string("[Error] JSON parse error (Ranks): ") + e.what());
        return false;
    } catch (const std::exception& e) {
        recordError(std::string("[Error] Failed to read ranks json: ") + e.what());
        return false;
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
    return loadedCount > 0;
}

int GameDatabase::getRankChips(Rank rank) const {
    int r = static_cast<int>(rank);
    auto it = m_rankChips.find(r);
    if (it != m_rankChips.end()) {
        return it->second;
    }
    std::cerr << "[Warning] No chip value found for Rank index: " << r << " (Check ranks.json)" << std::endl;
    return 0;
}

std::vector<std::string> GameDatabase::getAllJokerIds() const {
    std::vector<std::string> ids;
    ids.reserve(m_jokerDb.size());

    for (const auto& [key, _] : m_jokerDb) {
        ids.push_back(key);
    }
    return ids;
}
