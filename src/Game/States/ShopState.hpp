#pragma once

#include "IGameState.hpp"
#include <memory>

// 前置声明
class Game;
class Card;

/**
 * 商店状态 (ShopState)
 * * 处理购买 Joker、刷新商店和进入下一轮的逻辑。
 */
class ShopState : public IGameState {
public:
    void onEnter(Game& game) override;
    void onExit(Game& game) override;
    void handleEvent(Game& game, const sf::Event& event) override;
    void update(Game& game, float dt) override;
    void draw(Game& game, sf::RenderTarget& target) override;

private:
    // 当前准备购买（已选中）的商品卡牌
    std::weak_ptr<Card> m_pendingPurchase;

    /**
     * 商店进货
     * * 清空当前商店区域并随机生成新的 Joker 卡牌。
     * * @param game 游戏核心实例
     */
    void restockShop(Game& game);
};