#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

// 引入核心组件
#include "GameContext.hpp"
#include "../States/IGameState.hpp"
#include "../UI/UIManager.hpp"
#include "../UI/Tooltip.hpp"
#include "../UI/FloatingText.hpp"
#include "../Objects/CardArea.hpp"
#include "../Objects/Card.hpp" 

class Game {
public:
    Game();
    ~Game() = default;

    void run();

    // --- 状态管理 ---
    // 切换到新状态 (RunState, ShopState 等)
    void changeState(std::unique_ptr<IGameState> newState);

    // --- 公共 API (供 State 调用) ---
    
    // 获取游戏核心数据上下文
    GameContext& getContext() { return m_ctx; }
    
    // 获取 UI 管理器
    UIManager& getUI() { return m_ui; }
    
    // 获取窗口引用 (用于鼠标坐标计算等)
    sf::RenderWindow& getWindow() { return m_window; }

    // 生成飘字特效
    void spawnFloatingText(const std::string& text, sf::Vector2f pos, sf::Color color);

private:
    void initWindow();
    void initResources();
    void initScene();

    void processEvents();
    void update(float dt);
    void render();

    // --- 渲染相关 ---
    sf::RenderWindow m_window;
    
    // CRT 管线
    sf::RenderTexture m_renderTexture;
    sf::Shader m_crtShader;
    bool m_shaderLoaded = false;
    float m_shaderTime = 0.0f;

    // --- UI & 特效 ---
    UIManager m_ui;
    Tooltip m_tooltip;
    bool m_showTooltip = false;
    std::vector<FloatingText> m_effects;

    // --- 核心数据 ---
    GameContext m_ctx;

    // --- 场景对象 (持有权在 Game，Context 持有引用) ---
    std::shared_ptr<CardArea> m_handArea;
    std::shared_ptr<CardArea> m_jokerArea;
    std::shared_ptr<CardArea> m_shopArea;
    
    // --- 交互状态 ---
    // 鼠标当前悬停的卡牌 (用于 Tooltip 和 缩放动画)
    // [修复] 这是之前报错未定义的变量
    std::shared_ptr<Card> m_hoveredCard = nullptr;

    // --- 当前状态 (状态机) ---
    std::unique_ptr<IGameState> m_currentState;
};