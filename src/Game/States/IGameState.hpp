#pragma once
#include <SFML/Graphics.hpp>

// 前置声明，避免循环依赖
class Game;

class IGameState {
public:
    virtual ~IGameState() = default;

    // 进入该状态时触发 (类似于 startRound 或 restockShop)
    virtual void onEnter(Game& game) {}
    
    // 退出该状态时触发
    virtual void onExit(Game& game) {}

    // 处理输入 (替代 processEvents 中的 if-else)
    virtual void handleEvent(Game& game, const sf::Event& event) = 0;

    // 逻辑更新 (替代 update 中的 if-else)
    virtual void update(Game& game, float dt) = 0;

    // 绘制 (替代 render 中的 if-else)
    // 注意：只负责绘制该状态特有的内容到 renderTexture，通用UI由Game负责
    virtual void draw(Game& game, sf::RenderTarget& target) = 0;
};