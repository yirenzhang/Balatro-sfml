#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include <optional>
#include "../Objects/Card.hpp" // 为了使用 Suit 和 Rank 枚举
#include "../Systems/GameDatabase.hpp" // 为了获取初始筹码值

// 轻量级结构，仅存储数据，不涉及渲染
struct CardData {
    Suit suit;
    Rank rank;
    int baseChips;
    // 未来可以在这里扩展：
    // bool isGold = false;
    // bool isSteel = false;
    // string enhancementId; 
};

class Deck {
public:
    Deck() {
        
    }

    // 初始化标准 52 张牌
    void initStandardDeck() {
        m_cards.clear();
        for (int s = 0; s < 4; ++s) {
            for (int r = 2; r <= 14; ++r) {
                CardData data;
                data.suit = (Suit)s;
                data.rank = (Rank)r;
                // 从数据库查询该点数的默认筹码值
                data.baseChips = GameDatabase::Instance().getRankChips(data.rank);
                m_cards.push_back(data);
            }
        }
    }

    // 洗牌
    void shuffle() {
        static std::random_device rd;
        static std::mt19937 g(rd());
        std::shuffle(m_cards.begin(), m_cards.end(), g);
    }

    // 抽牌 (从末尾移除)
    std::optional<CardData> draw() {
        if (m_cards.empty()) {
            return std::nullopt; // 没牌了
        }
        CardData top = m_cards.back();
        m_cards.pop_back();
        return top;
    }

    // 获取剩余数量
    int getRemainingCount() const {
        return (int)m_cards.size();
    }

    // [拓展接口] 往牌堆里加牌 (比如使用了“幻灵牌”生成了一张新牌)
    void addCard(Suit s, Rank r) {
        CardData data;
        data.suit = s;
        data.rank = r;
        data.baseChips = GameDatabase::Instance().getRankChips(r);
        m_cards.push_back(data);
    }

private:
    std::vector<CardData> m_cards;
};