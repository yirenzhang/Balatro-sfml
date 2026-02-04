#include "Game.hpp"
#include "../States/RunState.hpp"
#include "../States/ShopState.hpp"
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
    // 对应 CRT.fs: uniform float time;
    if (m_shaderLoaded) {
        m_shaderTime += dt;
        // 简单防止溢出，每 1000 秒重置一次 (Shader 里的 sin/cos 是循环的，不影响效果)
        if (m_shaderTime > 1000.0f) m_shaderTime = 0.0f;
        
        // 注意：这里其实不需要 setUniform，因为我们在 render 时会统一设置
        // 但为了保险起见，或者如果 update 里有其他逻辑依赖，留着也无妨。
        // 为了代码整洁，建议移到 render 中统一处理。
    }

    // 2. 物理/动画更新
    // 必须调用 m_jokerArea->update，否则动画不播放
    if (m_jokerArea) m_jokerArea->update(dt);

    if (m_currentState) {
        m_currentState->update(*this, dt);
    }

    // 3. UI 更新
    m_ui.update(m_ctx);

    // 4. 特效更新与清理
    for (auto& effect : m_effects) {
        effect.update(dt);
    }
    // C++20/Standard remove_if idioms
    m_effects.erase(
        std::remove_if(m_effects.begin(), m_effects.end(), 
            [](const FloatingText& ft){ return ft.isDead(); }),
        m_effects.end()
    );

    // 5. 鼠标交互逻辑 (保持你原有的代码不变...)
    // ... (省略鼠标悬停检测代码，直接用你原来的即可) ...
    sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
    std::shared_ptr<Card> currentHovered = nullptr;
    if (m_jokerArea) currentHovered = m_jokerArea->getCardAt(mousePos.x, mousePos.y);
    if (!currentHovered) {
        if (m_ctx.state == GameState::Run && m_handArea) currentHovered = m_handArea->getCardAt(mousePos.x, mousePos.y);
        else if (m_ctx.state == GameState::Shop && m_shopArea) currentHovered = m_shopArea->getCardAt(mousePos.x, mousePos.y);
    }
    if (currentHovered != m_hoveredCard) {
        if (m_hoveredCard) m_hoveredCard->setHover(false);
        if (currentHovered) currentHovered->setHover(true);
        m_hoveredCard = currentHovered;
    }

    // 6. Tooltip
    m_showTooltip = false;
    if (m_hoveredCard) {
        m_showTooltip = true;
        if (m_hoveredCard->getType() == CardType::Joker) {
            m_tooltip.update(m_hoveredCard->getAbilityName(), m_hoveredCard->getDescription(), mousePos);
        } else {
            m_tooltip.update("Playing Card", "Chips: " + std::to_string(m_hoveredCard->getChips()), mousePos);
        }
    }
    
    // --- [新增] 快捷键调试 CRT 参数 (可选) ---
    // 按上/下键调整扫描线密度
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) m_crtParams.scanlines += 10.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) m_crtParams.scanlines -= 10.0f;
}

void Game::render() {
    // --------------------------------------------------------
    // 第一阶段：离屏绘制 (Off-screen Rendering)
    // 所有内容先画到 m_renderTexture
    // --------------------------------------------------------
    
    m_renderTexture.clear(sf::Color(35, 35, 40));

    // 1. 绘制 Joker 区域
    if (m_jokerArea) m_jokerArea->draw(m_renderTexture);

    // 2. 绘制状态特定内容
    if (m_currentState) {
        m_currentState->draw(*this, m_renderTexture);
    }

    // 3. 绘制特效
    for (auto& effect : m_effects) effect.draw(m_renderTexture);

    // 4. [重要] 绘制 UI 和 Tooltip 到 RenderTexture
    // 这样 UI 也会有 CRT 效果 (这符合 Balatro 风格)
    if (m_currentState) {
        // 修正：让 UI 画在 texture 上，而不是 window 上
        // 这需要你的 UIManager::draw 支持 sf::RenderTarget& 参数 (之前已做过修改)
        m_ui.draw(m_renderTexture, m_ctx.state);
    }
    
    if (m_showTooltip) {
        m_tooltip.draw(m_renderTexture);
    }
    
    m_renderTexture.display();
    
    // --------------------------------------------------------
    // 第二阶段：上屏 (On-screen Display)
    // 应用 Shader 并绘制到窗口
    // --------------------------------------------------------

    m_window.clear();
    sf::Sprite canvas(m_renderTexture.getTexture());

    if (m_shaderLoaded) {
        // --- 1. 基础环境参数 ---
        
        // [关键] 显式传递纹理
        // 对应 CRT.fs: uniform sampler2D texture;
        m_crtShader.setUniform("texture", sf::Shader::CurrentTexture);
        
        // [关键] 传递时间
        // 对应 CRT.fs: uniform float time;
        m_crtShader.setUniform("time", m_shaderTime);
        
        // [关键] 传递分辨率
        // 对应 CRT.fs: uniform vec2 resolution;
        sf::Vector2f texSize(
            static_cast<float>(m_renderTexture.getSize().x), 
            static_cast<float>(m_renderTexture.getSize().y)
        );
        m_crtShader.setUniform("resolution", texSize);

        // --- 2. 应用高级参数 ---
        // 使用我们封装的结构体，一键应用所有 "factor" 参数
        m_crtParams.applyTo(m_crtShader);

        // 绘制！
        m_window.draw(canvas, &m_crtShader);
    } else {
        // 如果 Shader 没加载成功，直接画原图
        m_window.draw(canvas);
    }

    m_window.display();
}

void Game::spawnFloatingText(const std::string& text, sf::Vector2f pos, sf::Color color) {
    m_effects.emplace_back(text, pos, color, ResourceManager::Instance().getFont("main"));
}