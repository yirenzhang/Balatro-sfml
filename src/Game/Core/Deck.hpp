#pragma once
#include <vector>
#include <algorithm>
#include <cstddef>
#include <random>
#include <optional>
#include <functional>
#include "../Objects/Card.hpp" // 为了使用 Suit 和 Rank 枚举

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
    using RankChipProvider = std::function<int(Rank)>;

    Deck() = default;

    void setRankChipProvider(RankChipProvider provider) {
        m_rankChipProvider = std::move(provider);
    }

    // 初始化标准 52 张牌
    void initStandardDeck() {
        m_cards.clear();
        for (int s = 0; s < 4; ++s) {
            for (int r = 2; r <= 14; ++r) {
                CardData data;
                data.suit = (Suit)s;
                data.rank = (Rank)r;
                data.baseChips = getBaseChips(data.rank);
                m_cards.push_back(data);
            }
        }
    }

    // 洗牌
    void shuffle(const std::function<std::size_t(std::size_t)>& pickIndex = {}) {
        if (m_cards.size() < 2) return;

        if (pickIndex) {
            for (std::size_t i = m_cards.size() - 1; i > 0; --i) {
                const std::size_t j = pickIndex(i + 1) % (i + 1);
                std::swap(m_cards[i], m_cards[j]);
            }
            return;
        }

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
        data.baseChips = getBaseChips(r);
        m_cards.push_back(data);
    }

private:
    int getBaseChips(Rank rank) const {
        if (m_rankChipProvider) {
            return m_rankChipProvider(rank);
        }
        return 0;
    }

    std::vector<CardData> m_cards;
    RankChipProvider m_rankChipProvider;
};
