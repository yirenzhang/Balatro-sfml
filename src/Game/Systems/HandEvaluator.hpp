#pragma once
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "../Objects/CardModel.hpp"
#include "CardSnapshot.hpp"

/**
 * 标准牌型枚举。
 *
 * 保留扩展牌型枚举值是为了与未来 Joker 规则兼容，
 * 避免后续引入新牌型时修改二进制布局。
 */
enum class PokerHandType {
    HighCard,
    Pair,
    TwoPair,
    ThreeOfAKind,
    Straight,
    Flush,
    FullHouse,
    FourOfAKind,
    StraightFlush,
    RoyalFlush,
    FiveOfAKind,
    FlushFive,
    FlushHouse
};

/**
 * 牌型评估结果。
 */
struct HandResult {
    PokerHandType type;
    std::string name;
    int base_chips;
    int base_mult;

    // 计分快照用于后续效果链路，避免直接传递渲染对象。
    std::vector<CardSnapshot> scoring_snapshots;
};

class HandEvaluator {
public:
    /**
     * 评估输入手牌并返回计分基础值。
     *
     * @param hand 手牌快照
     * @return 评估结果
     */
    static HandResult Evaluate(const std::vector<CardSnapshot>& hand);

private:
    /**
     * 判断是否同花。
     *
     * @param hand 手牌快照
     * @return 是否同花
     */
    static bool isFlush(const std::vector<CardSnapshot>& hand);

    /**
     * 判断是否顺子。
     *
     * @param hand 手牌快照
     * @return 是否顺子
     */
    static bool isStraight(const std::vector<CardSnapshot>& hand);
};
