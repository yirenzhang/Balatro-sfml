#include "Game.hpp"
#include "../States/RunState.hpp"
#include "../Systems/GameDatabase.hpp"
#include "../Systems/ResourceManager.hpp"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <algorithm>

Game::Game() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    initWindow();
    m_bootstrapReady = initResources();
    if (m_bootstrapReady) {
        initScene();
    } else {
        std::cerr << "[Fatal] Startup failed due to missing critical assets/data." << std::endl;
    }
}

// [New] 状态切换核心函数
void Game::changeState(std::unique_ptr<IGameState> newState) {
    m_stateMachine.changeState(*this, std::move(newState));
}

void Game::initWindow() {
    m_window.create(sf::VideoMode(1280, 720), "Balatro C++ State Pattern");
    m_window.setFramerateLimit(60);
    m_renderPipeline.init(1280, 720);
}

bool Game::initResources() {
    auto& res = m_resources;
    auto& db = m_database;
    res.clearErrors();
    db.clearErrors();

    m_ctx.resources = &res;
    m_ctx.database = &db;
    db.setResourceManager(&res);

    // Shader 加载
    const bool shaderLoaded = m_renderPipeline.loadShader("assets/shaders/CRT.fs");
    
    // 资源加载
    const bool deckLoaded = res.loadTexture("deck", "assets/textures/1x/8BitDeck.png");
    const bool jokersLoaded = res.loadTexture("jokers", "assets/textures/1x/Jokers.png");

    bool fontLoaded = res.loadFont("main", "assets/fonts/m6x11plus.ttf");
    if (!fontLoaded) {
        fontLoaded = res.loadFont("main", "C:/Windows/Fonts/arial.ttf");
    }

    const bool ranksLoaded = db.loadRanks("assets/data/ranks.json");
    const bool jokersDbLoaded = db.loadJokers("assets/data/jokers.json");
    m_ctx.deck.setRankChipProvider(
        [dbPtr = m_ctx.database](Rank rank) {
            return dbPtr ? dbPtr->getRankChips(rank) : 0;
        }
    );

    const StartupInputs inputs{
        .shaderLoaded = shaderLoaded,
        .deckTextureLoaded = deckLoaded,
        .jokersTextureLoaded = jokersLoaded,
        .fontLoaded = fontLoaded,
        .ranksLoaded = ranksLoaded,
        .jokersDbLoaded = jokersDbLoaded,
    };
    const StartupDecision decision = StartupPolicy::Evaluate(inputs);

    if (!decision.canStart) {
        std::cerr << "[Error] Startup blocked by critical failures:" << std::endl;
        for (const auto& msg : decision.errors) {
            std::cerr << "  - " << msg << std::endl;
        }
    }

    if (decision.degraded) {
        for (const auto& msg : decision.warnings) {
            std::cerr << "[Warning] " << msg << std::endl;
        }
    }

    if (!res.errors().empty() || !db.errors().empty()) {
        std::cerr << "[Error] Initialization diagnostics:" << std::endl;
        for (const auto& err : res.errors()) {
            std::cerr << "  - " << err << std::endl;
        }
        for (const auto& err : db.errors()) {
            std::cerr << "  - " << err << std::endl;
        }
    }

    // UI 初始化
    if (res.hasFont("main")) {
        m_ui.init(res.getFont("main"));
        m_tooltip.init(res.getFont("main"));
    }
    return decision.canStart;
}

void Game::initScene() {
    m_scene.initDefaultLayout(m_ctx, 1280.0f, 720.0f);

    // 启动初始状态：进入 RunState
    changeState(std::make_unique<RunState>());
}

void Game::run() {
    if (!m_bootstrapReady) {
        return;
    }
    sf::Clock clock;
    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    m_inputRouter.process(m_window, [this](const sf::Event& event) {
        if (auto* state = m_stateMachine.currentState()) {
            state->handleEvent(*this, event);
        }
    });
}

void Game::update(float dt) {
    m_renderPipeline.update(dt);

    // 物理/动画更新
    if (auto* jokerArea = m_scene.jokerArea()) jokerArea->update(dt);

    if (auto* state = m_stateMachine.currentState()) {
        state->update(*this, dt);
    }

    // UI 更新
    m_ui.update(m_ctx);

    // 特效更新与清理
    for (auto& effect : m_effects) {
        effect.update(dt);
    }
    // C++20/Standard remove_if idioms
    m_effects.erase(
        std::remove_if(m_effects.begin(), m_effects.end(), 
            [](const FloatingText& ft){ return ft.isDead(); }),
        m_effects.end()
    );

    m_hoverTooltip.update(m_window, m_scene, m_ctx, m_tooltip);
}

void Game::render() {
    m_renderPipeline.beginFrame(sf::Color(35, 35, 40));
    auto& target = m_renderPipeline.target();

    if (auto* jokerArea = m_scene.jokerArea()) jokerArea->draw(target);

    if (auto* state = m_stateMachine.currentState()) {
        state->draw(*this, target);
    }

    for (auto& effect : m_effects) effect.draw(target);

    if (m_stateMachine.currentState()) {
        m_ui.draw(target, m_ctx.state);
    }
    
    if (m_hoverTooltip.showTooltip()) {
        m_tooltip.draw(target);
    }

    m_renderPipeline.endFrame();
    m_renderPipeline.present(m_window, m_crtParams);
}

void Game::spawnFloatingText(const std::string& text, sf::Vector2f pos, sf::Color color) {
    if (!m_ctx.resources) return;
    m_effects.emplace_back(text, pos, color, m_ctx.resources->getFont("main"));
}
