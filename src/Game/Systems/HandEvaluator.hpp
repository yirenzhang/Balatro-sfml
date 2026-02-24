#pragma once
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "../Objects/CardModel.hpp"
#include "CardSnapshot.hpp"

// Balatro 的标准牌型
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
    FiveOfAKind,     // 需要 Joker 支持
    FlushFive,       // 需要 Joker 支持
    FlushHouse       // 需要 Joker 支持
};

// 评估结果
struct HandResult {
    PokerHandType type;
    std::string name;
    int base_chips;
    int base_mult;
    
    // 轻量快照，供规则层使用，避免直接依赖渲染对象
    std::vector<CardSnapshot> scoring_snapshots;
};

class HandEvaluator {
public:
    // 核心函数：输入一组牌，返回牌型和计分数据
    static HandResult Evaluate(const std::vector<CardSnapshot>& hand);

private:
    // 辅助检查函数
    static bool isFlush(const std::vector<CardSnapshot>& hand);
    static bool isStraight(const std::vector<CardSnapshot>& hand);
};
