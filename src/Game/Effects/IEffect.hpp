#pragma once
#include <memory>
#include <optional>
#include "EffectContext.hpp"

/**
 * 效果统一接口。
 *
 * 通过统一接口约束效果输入输出，可以在计分链路中
 * 以同一方式组合不同 joker 效果。
 */
class IEffect {
public:
    virtual ~IEffect() = default;

    /**
     * 计算效果结果。
     *
     * @param self 拥有效果的卡牌
     * @param ctx 当前触发上下文
     * @return 若触发返回结果，否则返回 std::nullopt
     */
    virtual std::optional<EffectResult> Calculate(
        const Card& self, 
        const EffectContext& ctx
    ) = 0;
};
