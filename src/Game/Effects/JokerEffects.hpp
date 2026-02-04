#pragma once
#include "IEffect.hpp"
#include <string>
#include <iostream>
#include "../Objects/CardArea.hpp" 

// --- 基础效果 (Joker) ---
// 适用：Joker (+4 Mult)
class SimpleMultEffect : public IEffect {
public:
    SimpleMultEffect(int amount) : m_amount(amount) {}

    std::optional<EffectResult> Calculate([[maybe_unused]] const Card& self, const EffectContext& ctx) override {
        // Lua: if context.joker_main then return {mult_mod = ...}
        if (ctx.trigger == TriggerType::Global) {
            EffectResult res;
            res.triggered = true;
            res.mult_add = m_amount;
            res.message = "+" + std::to_string(m_amount) + " Mult";
            return res;
        }
        return std::nullopt;
    }
private:
    int m_amount;
};

// --- 花色加成效果 (Greedy, Lusty 等) ---
// 适用：Greedy Joker (+4 Mult if Diamonds)
class SuitMultEffect : public IEffect {
public:
    SuitMultEffect(int amount, Suit suit, std::string suitName) 
        : m_amount(amount), m_suit(suit), m_suitName(suitName) {}

    std::optional<EffectResult> Calculate([[maybe_unused]] const Card& self, const EffectContext& ctx) override {
        // Lua: if context.individual and context.cardarea == G.play then
        //      if context.other_card:is_suit(...) then ...
        
        if (ctx.trigger == TriggerType::Individual && ctx.other_card != nullptr) {
            // 检查正在计分的那张牌是否符合花色
            if (ctx.other_card->getSuit() == m_suit) {
                EffectResult res;
                res.triggered = true;
                res.mult_add = m_amount;
                res.message = "+" + std::to_string(m_amount) + " Mult (" + m_suitName + ")";
                return res;
            }
        }
        return std::nullopt;
    }

private:
    // [之前丢失的部分] 成员变量定义
    int m_amount;
    Suit m_suit;
    std::string m_suitName;
};

// --- 抽象小丑 (Abstract Joker) ---
// 适用：Abstract Joker (+3 Mult per Joker)
class AbstractJokerEffect : public IEffect {
public:
    AbstractJokerEffect(int amountPerJoker) : m_amount(amountPerJoker) {}

    std::optional<EffectResult> Calculate([[maybe_unused]] const Card& self, const EffectContext& ctx) override {
        // Lua: if self.ability.name == 'Abstract Joker' then ...
        if (ctx.trigger == TriggerType::Global && ctx.joker_area) {
            int jokerCount = 0;
            // 统计 Joker 区有多少张牌
            jokerCount = (int)ctx.joker_area->getCards().size();
            
            int totalAdd = jokerCount * m_amount;
            
            EffectResult res;
            res.triggered = true;
            res.mult_add = totalAdd;
            res.message = "+" + std::to_string(totalAdd) + " Mult (" + std::to_string(jokerCount) + " Jokers)";
            return res;
        }
        return std::nullopt;
    }
private:
    int m_amount;
};

// --- 弃牌返利效果 (Discard Rebate) ---
// 适用：Mail-In Rebate (弃掉特定 Rank 给钱)
class DiscardRebateEffect : public IEffect {
public:
    DiscardRebateEffect(int dollars, Rank targetRank) 
        : m_dollars(dollars), m_targetRank(targetRank) {}

    std::optional<EffectResult> Calculate([[maybe_unused]] const Card& self, const EffectContext& ctx) override {
        // 检查触发时机：必须是弃牌阶段 (OnDiscard)
        // 并且必须是针对单张牌 (Individual，此时 other_card 不为空)
        if (ctx.trigger == TriggerType::OnDiscard && ctx.other_card != nullptr) {
            
            // 检查被弃掉的牌是否符合条件
            if (ctx.other_card->getRank() == m_targetRank) {
                EffectResult res;
                res.triggered = true;
                res.dollars_add = m_dollars;
                res.message = "+$" + std::to_string(m_dollars);
                return res;
            }
        }
        return std::nullopt;
    }

private:
    int m_dollars;
    Rank m_targetRank;
};