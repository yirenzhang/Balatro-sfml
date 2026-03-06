#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>
#include <random>
#include <vector>

#include "../Objects/Card.hpp"

/**
 * 牌堆内部使用的轻量牌数据。
 *
 * 与渲染对象解耦后，洗牌与抽牌规则可以独立测试，
 * 从而降低玩法层受图形资源生命周期影响的概率。
 */
struct CardData {
    Suit suit;
    Rank rank;
    int baseChips;
};

class Deck {
public:
    using RankChipProvider = std::function<int(Rank)>;

    Deck() = default;

    /**
     * 注入点数到筹码值的映射函数。
     *
     * 通过外部提供器驱动基础值，便于后续调整平衡而不改牌堆实现。
     *
     * @param provider 点数筹码查询函数
     */
    void setRankChipProvider(RankChipProvider provider) {
        m_rankChipProvider = std::move(provider);
    }

    /**
     * 初始化标准 52 张牌。
     *
     * 每次都会清空旧数据，避免上一轮残留牌进入新局。
     */
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

    /**
     * 原地洗牌。
     *
     * 提供 `pickIndex` 时走可控 Fisher-Yates，用于稳定回放或测试。
     * 不提供时走随机设备驱动的常规洗牌。
     *
     * @param pickIndex 可选索引生成函数
     */
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

    /**
     * 抽取一张牌。
     *
     * 使用尾部弹出可避免头部删除产生的线性移动开销。
     *
     * @return 抽到的牌；为空表示牌堆耗尽
     */
    std::optional<CardData> draw() {
        if (m_cards.empty()) {
            return std::nullopt;
        }
        CardData top = m_cards.back();
        m_cards.pop_back();
        return top;
    }

    /**
     * 获取剩余牌数。
     *
     * @return 当前牌堆剩余张数
     */
    int getRemainingCount() const {
        return static_cast<int>(m_cards.size());
    }

    /**
     * 追加一张牌到牌堆。
     *
     * 该入口保留给未来“效果生成新牌”等玩法扩展。
     *
     * @param s 花色
     * @param r 点数
     */
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
