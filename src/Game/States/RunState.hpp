#pragma once

#include "IGameState.hpp"
#include <vector>

// 前置声明，减少头文件依赖
class Game;
class Card;
struct GameContext;

/**
 * 游戏运行状态 (RunState)
 * * 处理核心的出牌逻辑、手牌管理和计分流程。
 * 也是游戏的主要循环状态。
 */
class RunState : public IGameState {
public:
    // 继承自 IGameState 的接口实现
    void onEnter(Game& game) override;
    void onExit(Game& game) override;
    void handleEvent(Game& game, const sf::Event& event) override;
    void update(Game& game, float dt) override;
    void draw(Game& game, sf::RenderTarget& target) override;

private:
    /**
     * 将手牌补充至上限
     * * @param game 游戏核心实例
     */
    void refillHand(Game& game);

    /**
     * 从手牌区域移除已选中的卡牌
     * * @param ctx 游戏上下文数据
     */
    void removeSelectedCards(GameContext& ctx);

    /**
     * 执行出牌逻辑
     * * 计算选中卡牌的分数，更新游戏状态，并在达到目标分时切换状态。
     * * @param game 游戏核心实例
     * @param selected 玩家选中的卡牌列表
     */
    void playHand(Game& game, std::vector<Card*> selected);
};