#include "HandEvaluator.hpp"
#include <iostream>

// Balatro 基础数值表 (Level 1)
// 实际项目中应从 planets.json 加载
static const std::map<PokerHandType, std::pair<int, int>> BASE_STATS = {
    {PokerHandType::HighCard,      {5, 1}},
    {PokerHandType::Pair,          {10, 2}},
    {PokerHandType::TwoPair,       {20, 2}},
    {PokerHandType::ThreeOfAKind,  {30, 3}},
    {PokerHandType::Straight,      {30, 4}},
    {PokerHandType::Flush,         {35, 4}},
    {PokerHandType::FullHouse,     {40, 4}},
    {PokerHandType::FourOfAKind,   {60, 7}},
    {PokerHandType::StraightFlush, {100, 8}},
    {PokerHandType::RoyalFlush,    {100, 8}}
};

HandResult HandEvaluator::Evaluate(const std::vector<Card*>& hand) {
    HandResult result;
    result.scoring_cards = hand; // 默认所有牌都计分 (后续修正)
    
    if (hand.empty()) {
        result.type = PokerHandType::HighCard;
        result.name = "High Card";
        result.base_chips = 0;
        result.base_mult = 0;
        return result;
    }

    // 1. 排序 (按 Rank 从小到大)
    std::vector<Card*> sorted = hand;
    std::sort(sorted.begin(), sorted.end(), [](Card* a, Card* b) {
        return a->getRank() < b->getRank();
    });

    // 2. 统计点数频率 (用于判断对子、三条等)
    std::map<Rank, int> rank_counts;
    std::map<Suit, int> suit_counts;
    for (auto* c : sorted) {
        rank_counts[c->getRank()]++;
        suit_counts[c->getSuit()]++;
    }

    // 3. 核心判断逻辑
    bool flush = isFlush(sorted);
    bool straight = isStraight(sorted);
    
    // 统计最大重复数
    int max_count = 0;
    int pair_count = 0;
    int three_count = 0;
    int four_count = 0;

    for (auto const& [rank, count] : rank_counts) {
        if (count > max_count) max_count = count;
        if (count == 2) pair_count++;
        if (count == 3) three_count++;
        if (count == 4) four_count++;
    }

    // --- 判定树 ---
    if (straight && flush) {
        // 判断是否是皇家同花顺 (最大牌是 Ace 且是顺子)
        if (sorted.back()->getRank() == Rank::Ace && sorted.front()->getRank() == Rank::Ten) {
             result.type = PokerHandType::RoyalFlush;
             result.name = "Royal Flush";
        } else {
             result.type = PokerHandType::StraightFlush;
             result.name = "Straight Flush";
        }
    }
    else if (four_count > 0) {
        result.type = PokerHandType::FourOfAKind;
        result.name = "4 of a Kind";
        // 修正计分牌：只保留那4张一样的
        result.scoring_cards.clear();
        for(auto* c : hand) if(rank_counts[c->getRank()] == 4) result.scoring_cards.push_back(c);
    }
    else if (three_count > 0 && pair_count > 0) {
        result.type = PokerHandType::FullHouse;
        result.name = "Full House";
    }
    else if (flush) {
        result.type = PokerHandType::Flush;
        result.name = "Flush";
    }
    else if (straight) {
        result.type = PokerHandType::Straight;
        result.name = "Straight";
    }
    else if (three_count > 0) {
        result.type = PokerHandType::ThreeOfAKind;
        result.name = "3 of a Kind";
        result.scoring_cards.clear();
        for(auto* c : hand) if(rank_counts[c->getRank()] == 3) result.scoring_cards.push_back(c);
    }
    else if (pair_count >= 2) {
        result.type = PokerHandType::TwoPair;
        result.name = "Two Pair";
        result.scoring_cards.clear();
        for(auto* c : hand) if(rank_counts[c->getRank()] == 2) result.scoring_cards.push_back(c);
    }
    else if (pair_count == 1) {
        result.type = PokerHandType::Pair;
        result.name = "Pair";
        result.scoring_cards.clear();
        for(auto* c : hand) if(rank_counts[c->getRank()] == 2) result.scoring_cards.push_back(c);
    }
    else {
        result.type = PokerHandType::HighCard;
        result.name = "High Card";
        // High Card 只计最大的那一张
        result.scoring_cards.clear();
        if (!sorted.empty()) result.scoring_cards.push_back(sorted.back());
    }

    // 4. 填充基础数值
    if (BASE_STATS.count(result.type)) {
        result.base_chips = BASE_STATS.at(result.type).first;
        result.base_mult = BASE_STATS.at(result.type).second;
    }

    return result;
}

bool HandEvaluator::isFlush(const std::vector<Card*>& hand) {
    if (hand.size() < 5) return false;
    Suit firstSuit = hand[0]->getSuit();
    for (auto* c : hand) {
        if (c->getSuit() != firstSuit) return false;
    }
    return true;
}

bool HandEvaluator::isStraight(const std::vector<Card*>& hand) {
    if (hand.size() < 5) return false;
    // 假设已排序
    
    // 特殊情况：A, 2, 3, 4, 5 (Ace is low)
    // hand[0]=2, ..., hand[3]=5, hand[4]=Ace
    bool lowAceStraight = (hand.back()->getRank() == Rank::Ace && 
                           hand[0]->getRank() == Rank::Two && 
                           hand[1]->getRank() == Rank::Three && 
                           hand[2]->getRank() == Rank::Four && 
                           hand[3]->getRank() == Rank::Five);
    if (lowAceStraight) return true;

    // 普通情况
    for (size_t i = 0; i < hand.size() - 1; ++i) {
        int diff = static_cast<int>(hand[i+1]->getRank()) - static_cast<int>(hand[i]->getRank());
        if (diff != 1) return false;
    }
    return true;
}