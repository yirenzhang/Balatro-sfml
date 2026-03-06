#pragma once

#include "IGameState.hpp"
#include "../Systems/CardSnapshot.hpp"
#include <vector>

class Game;
class Card;
struct GameContext;

class RunState : public IGameState {
public:
    /**
     * 进入运行状态。
     *
     * @param game 游戏宿主
     */
    void onEnter(Game& game) override;

    /**
     * 退出运行状态。
     *
     * @param game 游戏宿主
     */
    void onExit(Game& game) override;

    /**
     * 处理运行态输入。
     *
     * @param game 游戏宿主
     * @param event 输入事件
     */
    void handleEvent(Game& game, const sf::Event& event) override;

    /**
     * 更新运行态逻辑。
     *
     * @param game 游戏宿主
     * @param dt 帧间隔秒数
     */
    void update(Game& game, float dt) override;

    /**
     * 绘制运行态内容。
     *
     * @param game 游戏宿主
     * @param target 目标渲染器
     */
    void draw(Game& game, sf::RenderTarget& target) override;

private:
    /**
     * 获取当前已选牌快照缓存。
     *
     * 使用缓存是为了减少同一帧内重复构建快照的开销，
     * 保持输入判定和 UI 预览一致。
     *
     * @param ctx 运行上下文
     * @return 已选牌快照只读引用
     */
    const std::vector<CardSnapshot>& selectedSnapshots(GameContext& ctx);

    /**
     * 标记选牌缓存失效。
     */
    void markSelectionDirty() { m_selectionDirty = true; }

    /**
     * 将手牌补充到上限。
     *
     * @param game 游戏宿主
     */
    void refillHand(Game& game);

    /**
     * 从手牌区移除已选卡牌。
     *
     * @param ctx 运行上下文
     */
    void removeSelectedCards(GameContext& ctx);

    /**
     * 执行一次出牌结算。
     *
     * 将评估、计分、过关/失败迁移合并到单入口，
     * 目的是保证手牌消耗与分数推进原子一致。
     *
     * @param game 游戏宿主
     * @param selected 已选牌快照
     */
    void playHand(Game& game, std::vector<CardSnapshot> selected);

    std::vector<CardSnapshot> m_selectedCache;
    bool m_selectionDirty = true;
};
