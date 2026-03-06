#pragma once
#include <SFML/Graphics.hpp>

class Game;

class IGameState {
public:
    virtual ~IGameState() = default;

    /**
     * 进入状态时回调。
     *
     * 保留该生命周期钩子是为了把“状态准备动作”与常规更新分离，
     * 避免每帧重复初始化。
     *
     * @param game 游戏宿主
     */
    virtual void onEnter([[maybe_unused]] Game& game) {}
    
    /**
     * 退出状态时回调。
     *
     * @param game 游戏宿主
     */
    virtual void onExit([[maybe_unused]] Game& game) {}

    /**
     * 处理输入事件。
     *
     * @param game 游戏宿主
     * @param event 当前事件
     */
    virtual void handleEvent(Game& game, const sf::Event& event) = 0;

    /**
     * 执行状态逻辑更新。
     *
     * @param game 游戏宿主
     * @param dt 帧间隔秒数
     */
    virtual void update(Game& game, float dt) = 0;

    /**
     * 绘制状态内容。
     *
     * 仅绘制状态专属内容，通用 UI 由 Game 统一渲染，
     * 目的是避免各状态重复绘制同一 HUD。
     *
     * @param game 游戏宿主
     * @param target 目标渲染器
     */
    virtual void draw(Game& game, sf::RenderTarget& target) = 0;
};
