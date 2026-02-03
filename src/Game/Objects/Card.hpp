#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <cmath>
#include <memory> 

class IEffect; 

enum class Suit { Spades, Hearts, Clubs, Diamonds, None };
enum class Rank { 
    Two = 2, Three, Four, Five, Six, Seven, Eight, Nine, Ten, 
    Jack = 11, Queen = 12, King = 13, Ace = 14 
};

// 卡牌类型区分
enum class CardType {
    PlayingCard,
    Joker
};

class Card {
public:
    // 扑克牌 (8BitDeck.png) - 保持原样
    static constexpr int DECK_WIDTH = 71;
    static constexpr int DECK_HEIGHT = 95;
    
    // Joker (Jokers.png) - 根据你的描述
    static constexpr int JOKER_WIDTH = 69;
    static constexpr int JOKER_HEIGHT = 93;
    static constexpr int JOKER_GAP = 2;    // [New] 间距
    static constexpr int JOKER_MARGIN = 1; // [New] 边距

    // Joker 图集也是 10 列
    static constexpr int JOKER_COLS = 10;

    /**
     * @brief 构造函数 1: 普通扑克牌
     */
    Card(Suit suit, Rank rank, const sf::Texture& texture) 
        : m_type(CardType::PlayingCard), m_suit(suit), m_rank(rank) {
        
        initVisuals(texture, DECK_WIDTH, DECK_HEIGHT);
        
        // 计算扑克牌的 UV 坐标 (标准无间距排列)
        int gridX = static_cast<int>(rank) - 2; 
        int gridY = 0;
        switch (suit) {
            case Suit::Hearts:   gridY = 0; break;
            case Suit::Clubs:    gridY = 1; break;
            case Suit::Diamonds: gridY = 2; break;
            case Suit::Spades:   gridY = 3; break;
            default: break;
        }
        m_sprite.setTextureRect(sf::IntRect(gridX * DECK_WIDTH, gridY * DECK_HEIGHT, DECK_WIDTH, DECK_HEIGHT));
    }

    /**
     * @brief 构造函数 2: Joker 卡牌
     * @param jokerId Joker 在图集中的索引 (0, 1, 2...)
     * @param texture Joker.png 纹理
     */
    Card(int jokerId, const sf::Texture& texture)
        : m_type(CardType::Joker), m_suit(Suit::None), m_rank(Rank::Two) { // Joker 默认无花色点数
        
        initVisuals(texture, JOKER_WIDTH, JOKER_HEIGHT);

        // 计算 Joker 的 UV 坐标 (10列)
        int gridX = jokerId % JOKER_COLS;
        int gridY = jokerId / JOKER_COLS;

        // [修复] 根据 Sprite Sheet 的 Margin(1) 和 Gap(2) 计算精确坐标
        // x = margin + col * (width + gap)
        int rectX = JOKER_MARGIN + gridX * (JOKER_WIDTH + JOKER_GAP);
        int rectY = JOKER_MARGIN + gridY * (JOKER_HEIGHT + JOKER_GAP);

        m_sprite.setTextureRect(sf::IntRect(
            rectX, 
            rectY, 
            JOKER_WIDTH, 
            JOKER_HEIGHT
        ));
    }

    void update(float dt) {
        float visualOffsetY = 0.0f;
        // 动态缩放逻辑
        float visualScale = m_targetScale; 

        if (m_isSelected) {
            visualOffsetY = -30.0f; 
        } else if (m_isHovered) {
            visualOffsetY = -10.0f; 
            visualScale = m_targetScale + 0.1f;    
        }

        const float moveSpeed = 15.0f;
        const float scaleSpeed = 10.0f;

        sf::Vector2f currentPos = m_base.getPosition();
        sf::Vector2f finalTarget(m_targetPos.x, m_targetPos.y + visualOffsetY);

        sf::Vector2f newPos = currentPos + (finalTarget - currentPos) * moveSpeed * dt;
        
        float currentScale = m_scale;
        m_scale = currentScale + (visualScale - currentScale) * scaleSpeed * dt;

        m_base.setPosition(newPos);
        m_sprite.setPosition(newPos);
        
        // 应用缩放
        applyTransform();
        
        float shadowOffset = 5.0f + (-visualOffsetY * 0.2f); 
        m_shadow.setPosition(newPos.x + shadowOffset, newPos.y + shadowOffset);
    }

    void draw(sf::RenderTarget& target) {
        target.draw(m_shadow); 
        target.draw(m_base);   
        target.draw(m_sprite); 
    }

    void setTargetPosition(float x, float y) { m_targetPos = sf::Vector2f(x, y); }
    
    void setBaseScale(float scale) { 
        m_targetScale = scale; 
    }

    void setInstantPosition(float x, float y) {
        m_targetPos = sf::Vector2f(x, y);
        m_base.setPosition(x, y);
        m_sprite.setPosition(x, y);
        m_shadow.setPosition(x + 5.0f, y + 5.0f);
    }

    void setHover(bool hover) { m_isHovered = hover; }
    void toggleSelect() { m_isSelected = !m_isSelected; }
    void select(bool val) { m_isSelected = val; }
    bool isSelected() const { return m_isSelected; }
    sf::FloatRect getGlobalBounds() const { return m_base.getGlobalBounds(); }
    sf::Vector2f getPosition() const { 
        return m_base.getPosition(); 
    }
    Suit getSuit() const { return m_suit; }
    Rank getRank() const { return m_rank; }
    CardType getType() const { return m_type; }

    int getChips() const {
        if (m_type == CardType::Joker) return 0;
        return m_chips; 
    }
    void setChips(int chips) { m_chips = chips; }

    void setColor(const sf::Color& color) {
        m_sprite.setColor(color);
    }

    void setCost(int cost) { m_cost = cost; }
    int getCost() const { return m_cost; }

    void setEffect(std::shared_ptr<IEffect> effect) { m_effect = effect; }
    std::shared_ptr<IEffect> getEffect() const { return m_effect; }
    bool isJoker() const { return m_effect != nullptr; } 
    
    void setAbilityName(const std::string& name) { m_abilityName = name; }
    std::string getAbilityName() const { return m_abilityName; }

    void setDescription(const std::string& desc) { m_description = desc; }
    std::string getDescription() const { return m_description; }

private:
    // 提取公共初始化代码
    void initVisuals(const sf::Texture& texture, int w, int h) {
        m_base.setSize(sf::Vector2f((float)w, (float)h));
        m_base.setFillColor(sf::Color::White); 
        m_base.setOutlineColor(sf::Color::Black);
        m_base.setOutlineThickness(1.0f);
        m_base.setOrigin(w / 2.0f, h / 2.0f);

        m_shadow.setSize(sf::Vector2f((float)w, (float)h));
        m_shadow.setFillColor(sf::Color(0, 0, 0, 80));
        m_shadow.setOrigin(w / 2.0f, h / 2.0f);

        m_sprite.setTexture(texture);
        m_sprite.setOrigin(w / 2.0f, h / 2.0f);
        
        // 默认缩放
        m_targetScale = 3.0f;
        m_scale = 3.0f; 
        
        applyTransform();
    }
    
    void applyTransform() {
        m_base.setScale(m_scale, m_scale);
        m_sprite.setScale(m_scale, m_scale);
        m_shadow.setScale(m_scale, m_scale);
    }

    CardType m_type;
    Suit m_suit;
    Rank m_rank;
    
    sf::Sprite m_sprite;       
    sf::RectangleShape m_base; 
    sf::RectangleShape m_shadow;
    
    sf::Vector2f m_targetPos; 
    float m_targetScale = 3.0f; // 目标缩放大小
    float m_scale = 3.0f;       // 当前动态缩放

    bool m_isHovered = false;
    bool m_isSelected = false;

    std::shared_ptr<IEffect> m_effect = nullptr;
    std::string m_abilityName = "Card"; 

    std::string m_description = "";
    int m_chips = 0;
    int m_cost = 0;
};