#pragma once
#include <cassert>
#include "Deck.hpp"

class CardArea;
class GameDatabase;
class ResourceManager;

enum class GameState {
    Menu,
    Run,
    Shop,
    GameOver,
    Victory
};

struct GameContext {
    GameState state = GameState::Menu;
    
    Deck deck;
    GameDatabase* database = nullptr;
    ResourceManager* resources = nullptr;

    CardArea* area_hand = nullptr;
    CardArea* area_jokers = nullptr;
    CardArea* area_shop = nullptr; 

    int handsLeft = 4;
    int discardsLeft = 3;
    long long currentScore = 0;
    long long targetScore = 300;
    
    int money = 4; // 初始 4 块钱
    
    static const int HAND_SIZE_LIMIT = 8;

    bool hasHandArea() const { return area_hand != nullptr; }
    bool hasJokerArea() const { return area_jokers != nullptr; }
    bool hasShopArea() const { return area_shop != nullptr; }
    bool hasDatabase() const { return database != nullptr; }
    bool hasResources() const { return resources != nullptr; }

    CardArea& handArea() const {
        assert(area_hand && "GameContext::area_hand is null");
        return *area_hand;
    }

    CardArea& jokerArea() const {
        assert(area_jokers && "GameContext::area_jokers is null");
        return *area_jokers;
    }

    CardArea& shopArea() const {
        assert(area_shop && "GameContext::area_shop is null");
        return *area_shop;
    }

    GameDatabase& db() const {
        assert(database && "GameContext::database is null");
        return *database;
    }

    ResourceManager& res() const {
        assert(resources && "GameContext::resources is null");
        return *resources;
    }
};
