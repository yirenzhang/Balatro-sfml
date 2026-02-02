#pragma once
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "../Objects/Card.hpp"

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
    
    // 此次出牌中实际计分的牌 (例如两对只计算那4张牌，高牌不计分)
    std::vector<Card*> scoring_cards; 
};

class HandEvaluator {
public:
    // 核心函数：输入一组牌，返回牌型和计分数据
    static HandResult Evaluate(const std::vector<Card*>& hand);

private:
    // 辅助检查函数
    static bool isFlush(const std::vector<Card*>& hand);
    static bool isStraight(const std::vector<Card*>& hand);
};