#pragma once

#include "IGameState.hpp"
#include <memory>

class Game;
class Card;

class ShopState : public IGameState {
public:
    /**
     * 进入商店状态。
     *
     * @param game 游戏宿主
     */
    void onEnter(Game& game) override;

    /**
     * 退出商店状态。
     *
     * @param game 游戏宿主
     */
    void onExit(Game& game) override;

    /**
     * 处理商店输入。
     *
     * @param game 游戏宿主
     * @param event 输入事件
     */
    void handleEvent(Game& game, const sf::Event& event) override;

    /**
     * 更新商店逻辑。
     *
     * @param game 游戏宿主
     * @param dt 帧间隔秒数
     */
    void update(Game& game, float dt) override;

    /**
     * 绘制商店内容。
     *
     * @param game 游戏宿主
     * @param target 目标渲染器
     */
    void draw(Game& game, sf::RenderTarget& target) override;

private:
    // 记录待替换的商店牌，使用 weak_ptr 避免持有悬空对象。
    std::weak_ptr<Card> m_pendingPurchase;

    /**
     * 商店补货。
     *
     * 进入商店或刷新时统一走该入口，可确保库存逻辑一致。
     *
     * @param game 游戏宿主
     */
    void restockShop(Game& game);
};
