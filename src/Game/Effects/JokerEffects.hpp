#pragma once

#include <string>

#include "IEffect.hpp"
#include "../Objects/CardArea.hpp"

/**
 * 全局加倍率效果。
 */
class SimpleMultEffect : public IEffect {
public:
    /**
     * 构造效果。
     *
     * @param amount 触发时追加倍率
     */
    explicit SimpleMultEffect(int amount) : m_amount(amount) {}

    /**
     * 计算效果。
     *
     * 仅在全局阶段触发，目的是与逐牌阶段区分职责，避免重复加成。
     *
     * @param self 持有效果的卡牌
     * @param ctx 触发上下文
     * @return 触发结果
     */
    std::optional<EffectResult> Calculate([[maybe_unused]] const Card& self, const EffectContext& ctx) override {
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

/**
 * 花色条件倍率效果。
 */
class SuitMultEffect : public IEffect {
public:
    /**
     * 构造效果。
     *
     * @param amount 触发时追加倍率
     * @param suit 目标花色
     * @param suitName 展示名称
     */
    SuitMultEffect(int amount, Suit suit, std::string suitName)
        : m_amount(amount), m_suit(suit), m_suitName(std::move(suitName)) {}

    /**
     * 计算效果。
     *
     * 仅在逐牌阶段检查当前计分牌花色，保证与设计中的“单牌触发”一致。
     *
     * @param self 持有效果的卡牌
     * @param ctx 触发上下文
     * @return 触发结果
     */
    std::optional<EffectResult> Calculate([[maybe_unused]] const Card& self, const EffectContext& ctx) override {
        if (ctx.trigger == TriggerType::Individual &&
            ctx.has_other_card_snapshot &&
            ctx.other_card_snapshot.suit == m_suit) {
            EffectResult res;
            res.triggered = true;
            res.mult_add = m_amount;
            res.message = "+" + std::to_string(m_amount) + " Mult (" + m_suitName + ")";
            return res;
        }
        return std::nullopt;
    }

private:
    int m_amount;
    Suit m_suit;
    std::string m_suitName;
};

/**
 * 基于 Joker 数量叠加倍率的效果。
 */
class AbstractJokerEffect : public IEffect {
public:
    /**
     * 构造效果。
     *
     * @param amountPerJoker 每张 Joker 贡献的倍率
     */
    explicit AbstractJokerEffect(int amountPerJoker) : m_amount(amountPerJoker) {}

    /**
     * 计算效果。
     *
     * 在全局阶段统计 Joker 区数量，原因是该效果语义依赖“整体编队”。
     *
     * @param self 持有效果的卡牌
     * @param ctx 触发上下文
     * @return 触发结果
     */
    std::optional<EffectResult> Calculate([[maybe_unused]] const Card& self, const EffectContext& ctx) override {
        if (ctx.trigger == TriggerType::Global && ctx.joker_area) {
            const int jokerCount = static_cast<int>(ctx.joker_area->getCards().size());
            const int totalAdd = jokerCount * m_amount;

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

/**
 * 弃牌返利效果。
 */
class DiscardRebateEffect : public IEffect {
public:
    /**
     * 构造效果。
     *
     * @param dollars 每次触发返利金额
     * @param targetRank 目标点数
     */
    DiscardRebateEffect(int dollars, Rank targetRank)
        : m_dollars(dollars), m_targetRank(targetRank) {}

    /**
     * 计算效果。
     *
     * 仅在弃牌阶段且目标点数匹配时触发，避免在其他结算链路误加金钱。
     *
     * @param self 持有效果的卡牌
     * @param ctx 触发上下文
     * @return 触发结果
     */
    std::optional<EffectResult> Calculate([[maybe_unused]] const Card& self, const EffectContext& ctx) override {
        if (ctx.trigger == TriggerType::OnDiscard &&
            ctx.has_other_card_snapshot &&
            ctx.other_card_snapshot.rank == m_targetRank) {
            EffectResult res;
            res.triggered = true;
            res.dollars_add = m_dollars;
            res.message = "+$" + std::to_string(m_dollars);
            return res;
        }
        return std::nullopt;
    }

private:
    int m_dollars;
    Rank m_targetRank;
};
