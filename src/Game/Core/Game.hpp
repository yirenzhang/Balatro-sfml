#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

// 引入核心组件
#include "GameContext.hpp"
#include "StateMachine.hpp"
#include "InputRouter.hpp"
#include "RenderPipeline.hpp"
#include "SceneCoordinator.hpp"
#include "HoverTooltipController.hpp"
#include "../Systems/GameDatabase.hpp"
#include "../Systems/ResourceManager.hpp"
#include "../UI/UIManager.hpp"
#include "../UI/Tooltip.hpp"
#include "../UI/FloatingText.hpp"
#include "../Objects/CardArea.hpp"
#include "../Data/CRTParams.hpp"
#include "../Systems/StartupPolicy.hpp"

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

    CRTParams& getCRTParams() { return m_crtParams; }

private:
    void initWindow();
    bool initResources();
    void initScene();

    void processEvents();
    void update(float dt);
    void render();

    sf::RenderWindow m_window;
    InputRouter m_inputRouter;
    RenderPipeline m_renderPipeline;
    CRTParams m_crtParams;

    // --- UI & 特效 ---
    UIManager m_ui;
    Tooltip m_tooltip;
    HoverTooltipController m_hoverTooltip;
    std::vector<FloatingText> m_effects;

    // --- 核心数据 ---
    GameContext m_ctx;
    ResourceManager m_resources;
    GameDatabase m_database;

    // --- 场景对象协调器 ---
    SceneCoordinator m_scene;

    // --- 当前状态 (状态机) ---
    StateMachine m_stateMachine;
    bool m_bootstrapReady = true;
};
