#include "Game.hpp"
#include "../States/StateDefinitions.hpp" // 包含 ShopState 的具体实现
#include "../Systems/GameDatabase.hpp"
#include <iostream>
#include <ctime>
#include <algorithm>

Game::Game() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    initWindow();
    initResources();
    initScene();
}

// [New] 状态切换核心函数
void Game::changeState(std::unique_ptr<IGameState> newState) {
    if (m_currentState) {
        m_currentState->onExit(*this);
    }
    m_currentState = std::move(newState);
    if (m_currentState) {
        m_currentState->onEnter(*this);
    }
}

void Game::initWindow() {
    m_window.create(sf::VideoMode(1280, 720), "Balatro C++ State Pattern");
    m_window.setFramerateLimit(60);
    m_renderTexture.create(1280, 720);
}

void Game::initResources() {
    auto& res = ResourceManager::Instance();
    // Shader 加载 (省略参数设置细节，与之前一致)
    if (m_crtShader.loadFromFile("assets/shaders/CRT.fs", sf::Shader::Fragment)) {
        m_shaderLoaded = true;
        m_crtShader.setUniform("texture", sf::Shader::CurrentTexture);
        m_crtShader.setUniform("resolution", sf::Vector2f(1280, 720));
        // ... 其他 Shader 参数 ...
    }
    
    // 资源加载
    res.loadTexture("deck", "assets/textures/1x/8BitDeck.png");
    res.loadTexture("jokers", "assets/textures/1x/Jokers.png");
    if(!res.loadFont("main", "assets/fonts/m6x11plus.ttf")) res.loadFont("main", "C:/Windows/Fonts/arial.ttf");

    GameDatabase::Instance().loadRanks("assets/data/ranks.json");
    GameDatabase::Instance().loadJokers("assets/data/jokers.json");

    // UI 初始化
    m_ui.init();
    m_tooltip.init(res.getFont("main"));
}

void Game::initScene() {
    // 初始化各个区域 (CardArea)
    // 注意：不再需要在这里 refillHand 或 shuffle，
    // 因为这现在是 RunState::onEnter 的责任
    
    float jokerW = 600.0f, jokerH = 150.0f; 
    m_jokerArea = std::make_shared<CardArea>((1280.0f-jokerW)/2.0f, 30.0f, jokerW, jokerH, LayoutType::Row);
    m_ctx.area_jokers = m_jokerArea.get();

    float shopW = 600.0f, shopH = 150.0f;
    m_shopArea = std::make_shared<CardArea>((1280.0f-shopW)/2.0f, 300.0f, shopW, shopH, LayoutType::Row);
    m_ctx.area_shop = m_shopArea.get();

    float handW = 800.0f, handH = 280.0f;
    m_handArea = std::make_shared<CardArea>((1280.0f-handW)/2.0f, 720.0f-handH-30.0f, handW, handH, LayoutType::Fan);
    m_ctx.area_hand = m_handArea.get();

    // 启动初始状态：进入 RunState
    changeState(std::make_unique<RunState>());
}

void Game::run() {
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
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) m_window.close();
        
        // [New] 委托给当前状态处理事件
        if (m_currentState) {
            m_currentState->handleEvent(*this, event);
        }
    }
}

void Game::update(float dt) {
    // 1. Shader 时间更新
    if (m_shaderLoaded) {
        m_shaderTime += dt;
        m_crtShader.setUniform("time", m_shaderTime);
    }

    // 2. 物理/动画更新
    // [修复点] 全局更新 Joker 区域 (必须调用，否则商店买的牌飞不过去)
    if (m_jokerArea) m_jokerArea->update(dt);

    // 委托给当前状态更新逻辑 (RunState 更新手牌, ShopState 更新商品)
    if (m_currentState) {
        m_currentState->update(*this, dt);
    }

    // 3. 全局 UI 数据更新 (HUD, 分数等)
    m_ui.update(m_ctx);

    // 4. 漂浮文字特效更新与清理
    for (auto& effect : m_effects) {
        effect.update(dt);
    }
    // [注意] 需要 #include <algorithm>
    m_effects.erase(
        std::remove_if(m_effects.begin(), m_effects.end(), 
            [](const FloatingText& ft){ return ft.isDead(); }),
        m_effects.end()
    );

    // 5. 鼠标悬停检测
    // 获取鼠标在世界坐标系的位置
    sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
    
    std::shared_ptr<Card> currentHovered = nullptr;
    
    // 优先级 A: 始终检测 Joker 区域 (位于顶部)
    if (m_jokerArea) {
        currentHovered = m_jokerArea->getCardAt(mousePos.x, mousePos.y);
    }

    // 优先级 B: 如果没悬停 Joker，根据当前状态检测特定区域
    if (!currentHovered) {
        // 注意：这里依赖于 RunState/ShopState 在 onEnter 中正确设置了 m_ctx.state
        if (m_ctx.state == GameState::Run) {
            if (m_handArea) currentHovered = m_handArea->getCardAt(mousePos.x, mousePos.y);
        }
        else if (m_ctx.state == GameState::Shop) {
            if (m_shopArea) currentHovered = m_shopArea->getCardAt(mousePos.x, mousePos.y);
        }
    }

    // 处理悬停状态切换 (触发缩放动画)
    if (currentHovered != m_hoveredCard) {
        if (m_hoveredCard) m_hoveredCard->setHover(false); // 旧卡取消悬停
        if (currentHovered) currentHovered->setHover(true); // 新卡激活悬停
        m_hoveredCard = currentHovered;
    }

    // 6. Tooltip 数据更新
    m_showTooltip = false;
    if (m_hoveredCard) {
        m_showTooltip = true;
        
        if (m_hoveredCard->getType() == CardType::Joker) {
            // Joker 显示名字和能力描述
            m_tooltip.update(
                m_hoveredCard->getAbilityName(),
                m_hoveredCard->getDescription(),
                mousePos
            );
        }
        else {
            // 普通牌显示筹码信息
            std::string desc = "Chips: " + std::to_string(m_hoveredCard->getChips());
            m_tooltip.update("Playing Card", desc, mousePos);
        }
    }
}

void Game::render() {
    m_renderTexture.clear(sf::Color(35, 35, 40));

    // 1. 绘制 Joker 区域 (这是所有状态通用的)
    if (m_jokerArea) m_jokerArea->draw(m_renderTexture);

    // 2. [New] 委托给当前状态绘制特定内容 (如 RunState 画手牌，ShopState 画商品)
    if (m_currentState) {
        // std::cout << "Game: Calling state draw..." << std::endl; // [Debug 1]
        m_currentState->draw(*this, m_renderTexture);
    }

    // 3. 绘制特效
    for (auto& effect : m_effects) effect.draw(m_renderTexture);
    
    m_renderTexture.display();
    
    // 上屏
    m_window.clear();
    sf::Sprite canvas(m_renderTexture.getTexture());
    //if (m_shaderLoaded) m_window.draw(canvas, &m_crtShader);
    //else
    m_window.draw(canvas);

    // UI 绘制
    // 注意：现在的 UIManager 需要知道 State 吗？
    // 如果 UI 样式差异很大，可以让 State 自己调用 game.getUI().draw(...)
    // 或者我们仍然传递 enum (如果还保留的话)
    // 既然我们消灭了 enum，我们可以让 State 在它的 draw() 方法里调用 UI 绘制函数
    // 比如：RunState::draw() -> game.getUI().drawRunPanel()
    // 这里简单处理：让 UI 自行决定画什么，或者提供不同的 draw 方法
    if (m_currentState) {
        // 让 UI 知道当前是什么模式比较麻烦，除非使用 dynamic_cast
        // 简单方案：在 State::draw 里调用 game.getUI().drawHUD()
        m_ui.draw(m_window, GameState::Run); // 这里的枚举需要兼容或移除
    }
    
    if (m_showTooltip) m_tooltip.draw(m_window);
    m_window.display();
}

void Game::spawnFloatingText(const std::string& text, sf::Vector2f pos, sf::Color color) {
    m_effects.emplace_back(text, pos, color, ResourceManager::Instance().getFont("main"));
}