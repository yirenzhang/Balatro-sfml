#pragma once
#include <memory>
#include <optional>
#include "EffectContext.hpp"

// 效果接口
class IEffect {
public:
    virtual ~IEffect() = default;

    /**
     * @brief 计算效果
     * @param self 拥有此效果的卡牌 (自身)
     * @param ctx 上下文
     * @return 如果触发了，返回 EffectResult；否则返回 nullopt
     */
    virtual std::optional<EffectResult> Calculate(
        const Card& self, 
        const EffectContext& ctx
    ) = 0;
};