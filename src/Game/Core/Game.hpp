#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

// 引入核心类型
#include "GameContext.hpp"
#include "../Objects/CardArea.hpp"
#include "../Objects/Card.hpp"
#include "../UI/Tooltip.hpp"
#include "../UI/FloatingText.hpp"
#include "../Effects/JokerEffects.hpp"
#include "../Systems/ScoringManager.hpp" 

class Game {
public:
    Game();
    ~Game() = default;

    void run();

private:
    void initWindow();
    void initResources();
    void initScene();

    void processEvents();
    void update(float dt);
    void render();

    void refillHand();
    void restockShop();
    void startRound();

    // 生成飘字
    void spawnFloatingText(const std::string& text, sf::Vector2f pos, sf::Color color);

    // --- 核心 ---
    sf::RenderWindow m_window;
    
    // CRT 管线
    sf::RenderTexture m_renderTexture;
    sf::Shader m_crtShader;
    bool m_shaderLoaded = false;
    float m_shaderTime = 0.0f;

    // 资源
    sf::Texture m_textureDeck;
    sf::Texture m_textureJokers;
    sf::Font m_font;
    Tooltip m_tooltip;
    bool m_showTooltip = false;
    // UI 文本
    sf::Text m_textHUD;    // 显示 Hands: 4, Discards: 3
    sf::Text m_textScore;  // 显示 Score: 100 / 300
    sf::Text m_textShopInfo;
    sf::Text m_textDeckCount;

    // 牌型信息面板组件
    sf::RectangleShape m_handInfoBg;      // 整体背景
    sf::RectangleShape m_chipsBox;       // 蓝色筹码框
    sf::RectangleShape m_multBox;        // 红色倍率框
    
    sf::Text m_textHandType;             // 牌型名称 (如: 对子)
    sf::Text m_textHandLevel;            // 等级 (如: 等级1)
    sf::Text m_textBaseChips;            // 基础筹码数值
    sf::Text m_textBaseMult;             // 基础倍率数值
    sf::Text m_textMultSymbol;           // 中间的 "X" 符号

    std::vector<FloatingText> m_effects;

    // 数据上下文
    GameContext m_ctx;
    
    // 场景对象
    std::shared_ptr<CardArea> m_handArea;
    std::shared_ptr<CardArea> m_jokerArea;
    std::shared_ptr<CardArea> m_shopArea;

    std::shared_ptr<Card> m_pendingPurchase = nullptr;
    std::shared_ptr<Card> m_hoveredCard = nullptr;
};