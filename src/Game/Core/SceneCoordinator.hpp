#pragma once

#include <memory>

#include "../Objects/CardArea.hpp"
#include "GameContext.hpp"

class SceneCoordinator {
public:
    /**
     * 构建默认场景布局并回写上下文指针。
     *
     * 由统一协调器创建区域可以避免区域生命周期散落在 Game/State，
     * 降低状态切换时悬空指针风险。
     *
     * @param ctx 全局运行上下文
     * @param width 场景宽度
     * @param height 场景高度
     */
    void initDefaultLayout(GameContext& ctx, float width, float height);

    /**
     * 获取手牌区。
     *
     * @return 手牌区指针，可能为空
     */
    CardArea* handArea() const { return m_handArea.get(); }

    /**
     * 获取 Joker 区。
     *
     * @return Joker 区指针，可能为空
     */
    CardArea* jokerArea() const { return m_jokerArea.get(); }

    /**
     * 获取商店区。
     *
     * @return 商店区指针，可能为空
     */
    CardArea* shopArea() const { return m_shopArea.get(); }

private:
    std::shared_ptr<CardArea> m_handArea;
    std::shared_ptr<CardArea> m_jokerArea;
    std::shared_ptr<CardArea> m_shopArea;
};
