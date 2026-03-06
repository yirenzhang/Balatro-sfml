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
    
    int money = 4; // 初始资金较低，用于保留商店早期决策压力。
    
    static const int HAND_SIZE_LIMIT = 8;

    /**
     * 检查手牌区是否可用。
     *
     * @return 是否已绑定手牌区
     */
    bool hasHandArea() const { return area_hand != nullptr; }

    /**
     * 检查 Joker 区是否可用。
     *
     * @return 是否已绑定 Joker 区
     */
    bool hasJokerArea() const { return area_jokers != nullptr; }

    /**
     * 检查商店区是否可用。
     *
     * @return 是否已绑定商店区
     */
    bool hasShopArea() const { return area_shop != nullptr; }

    /**
     * 检查数据库是否可用。
     *
     * @return 是否已注入数据库
     */
    bool hasDatabase() const { return database != nullptr; }

    /**
     * 检查资源管理器是否可用。
     *
     * @return 是否已注入资源管理器
     */
    bool hasResources() const { return resources != nullptr; }

    /**
     * 获取手牌区引用。
     *
     * @return 手牌区引用
     */
    CardArea& handArea() const {
        assert(area_hand && "GameContext::area_hand is null");
        return *area_hand;
    }

    /**
     * 获取 Joker 区引用。
     *
     * @return Joker 区引用
     */
    CardArea& jokerArea() const {
        assert(area_jokers && "GameContext::area_jokers is null");
        return *area_jokers;
    }

    /**
     * 获取商店区引用。
     *
     * @return 商店区引用
     */
    CardArea& shopArea() const {
        assert(area_shop && "GameContext::area_shop is null");
        return *area_shop;
    }

    /**
     * 获取数据库引用。
     *
     * @return 数据库引用
     */
    GameDatabase& db() const {
        assert(database && "GameContext::database is null");
        return *database;
    }

    /**
     * 获取资源管理器引用。
     *
     * @return 资源管理器引用
     */
    ResourceManager& res() const {
        assert(resources && "GameContext::resources is null");
        return *resources;
    }
};
