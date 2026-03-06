#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

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
    /**
     * 构造游戏对象并完成启动初始化。
     */
    Game();
    ~Game() = default;

    /**
     * 启动主循环。
     *
     * 在统一入口执行主循环可以确保帧流程顺序固定，
     * 便于后续定位状态与渲染问题。
     */
    void run();

    /**
     * 切换状态。
     *
     * @param newState 新状态对象所有权
     */
    void changeState(std::unique_ptr<IGameState> newState);

    /**
     * 获取运行时上下文。
     *
     * @return 上下文引用
     */
    GameContext& getContext() { return m_ctx; }

    /**
     * 获取 UI 管理器。
     *
     * @return UI 管理器引用
     */
    UIManager& getUI() { return m_ui; }

    /**
     * 获取窗口对象。
     *
     * @return 窗口引用
     */
    sf::RenderWindow& getWindow() { return m_window; }

    /**
     * 生成飘字特效。
     *
     * @param text 显示文本
     * @param pos 位置
     * @param color 文本颜色
     */
    void spawnFloatingText(const std::string& text, sf::Vector2f pos, sf::Color color);

    /**
     * 获取 CRT 参数。
     *
     * @return CRT 参数引用
     */
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

    UIManager m_ui;
    Tooltip m_tooltip;
    HoverTooltipController m_hoverTooltip;
    std::vector<FloatingText> m_effects;

    GameContext m_ctx;
    ResourceManager m_resources;
    GameDatabase m_database;

    SceneCoordinator m_scene;

    StateMachine m_stateMachine;
    bool m_bootstrapReady = true;
};
