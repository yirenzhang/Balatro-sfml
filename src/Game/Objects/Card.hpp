#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

#include "CardModel.hpp"
#include "CardView.hpp"

class Card {
public:
    static constexpr int DECK_WIDTH = 71;
    static constexpr int DECK_HEIGHT = 95;
    static constexpr int JOKER_WIDTH = 69;
    static constexpr int JOKER_HEIGHT = 93;
    static constexpr int JOKER_GAP = 2;
    static constexpr int JOKER_MARGIN = 1;
    static constexpr int JOKER_COLS = 10;

    /**
     * 构造逻辑层扑克牌对象（无纹理）。
     *
     * 该构造用于纯逻辑场景，避免在无图形上下文环境下创建纹理依赖。
     *
     * @param suit 花色
     * @param rank 点数
     */
    Card(Suit suit, Rank rank) {
        m_model.type = CardType::PlayingCard;
        m_model.suit = suit;
        m_model.rank = rank;
    }

    /**
     * 构造可渲染扑克牌对象。
     *
     * @param suit 花色
     * @param rank 点数
     * @param texture 牌面图集
     */
    Card(Suit suit, Rank rank, const sf::Texture& texture) : Card(suit, rank) {

        m_view.init(texture, DECK_WIDTH, DECK_HEIGHT);

        const int gridX = static_cast<int>(rank) - 2;
        int gridY = 0;
        switch (suit) {
            case Suit::Hearts:   gridY = 0; break;
            case Suit::Clubs:    gridY = 1; break;
            case Suit::Diamonds: gridY = 2; break;
            case Suit::Spades:   gridY = 3; break;
            default: break;
        }
        m_view.setTextureRect(sf::IntRect(gridX * DECK_WIDTH, gridY * DECK_HEIGHT, DECK_WIDTH, DECK_HEIGHT));
    }

    /**
     * 构造逻辑层 Joker 对象（无纹理）。
     *
     * @param jokerId Joker 图集索引
     */
    Card([[maybe_unused]] int jokerId) {
        m_model.type = CardType::Joker;
        m_model.suit = Suit::None;
        m_model.rank = Rank::Two;
    }

    /**
     * 构造可渲染 Joker 对象。
     *
     * @param jokerId Joker 图集索引
     * @param texture Joker 图集纹理
     */
    Card(int jokerId, const sf::Texture& texture) : Card(jokerId) {

        m_view.init(texture, JOKER_WIDTH, JOKER_HEIGHT);

        const int gridX = jokerId % JOKER_COLS;
        const int gridY = jokerId / JOKER_COLS;
        const int rectX = JOKER_MARGIN + gridX * (JOKER_WIDTH + JOKER_GAP);
        const int rectY = JOKER_MARGIN + gridY * (JOKER_HEIGHT + JOKER_GAP);
        m_view.setTextureRect(sf::IntRect(rectX, rectY, JOKER_WIDTH, JOKER_HEIGHT));
    }

    /**
     * 更新卡牌动画状态。
     *
     * @param dt 帧间隔秒数
     */
    void update(float dt) { m_view.update(m_model.isSelected, m_model.isHovered, dt); }

    /**
     * 绘制卡牌。
     *
     * @param target 绘制目标
     */
    void draw(sf::RenderTarget& target) { m_view.draw(target); }

    /**
     * 设置目标位置。
     *
     * @param x X 坐标
     * @param y Y 坐标
     */
    void setTargetPosition(float x, float y) { m_view.setTargetPosition(x, y); }

    /**
     * 设置基础缩放。
     *
     * @param scale 缩放值
     */
    void setBaseScale(float scale) { m_view.setBaseScale(scale); }

    /**
     * 立即设置位置。
     *
     * @param x X 坐标
     * @param y Y 坐标
     */
    void setInstantPosition(float x, float y) { m_view.setInstantPosition(x, y); }

    /**
     * 设置悬停状态。
     *
     * @param hover 是否悬停
     */
    void setHover(bool hover) { m_model.isHovered = hover; }

    /**
     * 切换选中状态。
     */
    void toggleSelect() { m_model.isSelected = !m_model.isSelected; }

    /**
     * 显式设置选中状态。
     *
     * @param val 选中状态
     */
    void select(bool val) { m_model.isSelected = val; }
    /**
     * 读取选中状态。
     *
     * @return 是否选中
     */
    bool isSelected() const { return m_model.isSelected; }
    /**
     * 获取碰撞边界。
     *
     * @return 全局包围盒
     */
    sf::FloatRect getGlobalBounds() const { return m_view.getGlobalBounds(); }
    /**
     * 获取当前位置。
     *
     * @return 世界坐标
     */
    sf::Vector2f getPosition() const { return m_view.getPosition(); }
    /**
     * 获取花色。
     *
     * @return 花色
     */
    Suit getSuit() const { return m_model.suit; }
    /**
     * 获取点数。
     *
     * @return 点数
     */
    Rank getRank() const { return m_model.rank; }
    /**
     * 获取卡牌类型。
     *
     * @return 卡牌类型
     */
    CardType getType() const { return m_model.type; }

    /**
     * 获取筹码值。
     *
     * Joker 不参与牌面筹码，因此返回 0。
     *
     * @return 筹码值
     */
    int getChips() const {
        if (m_model.type == CardType::Joker) return 0;
        return m_model.chips;
    }
    /**
     * 设置筹码值。
     *
     * @param chips 筹码值
     */
    void setChips(int chips) { m_model.chips = chips; }

    /**
     * 设置渲染颜色。
     *
     * @param color 颜色值
     */
    void setColor(const sf::Color& color) { m_view.setColor(color); }

    /**
     * 设置购买价格。
     *
     * @param cost 价格
     */
    void setCost(int cost) { m_model.cost = cost; }
    /**
     * 获取购买价格。
     *
     * @return 价格
     */
    int getCost() const { return m_model.cost; }

    /**
     * 绑定效果对象。
     *
     * @param effect 效果所有权
     */
    void setEffect(std::shared_ptr<IEffect> effect) { m_model.effect = std::move(effect); }

    /**
     * 获取效果对象。
     *
     * @return 效果共享指针
     */
    std::shared_ptr<IEffect> getEffect() const { return m_model.effect; }

    /**
     * 判断是否为 Joker。
     *
     * @return 若绑定效果则视为 Joker
     */
    bool isJoker() const { return m_model.effect != nullptr; }

    /**
     * 设置能力名。
     *
     * @param name 能力名
     */
    void setAbilityName(const std::string& name) { m_model.abilityName = name; }

    /**
     * 获取能力名。
     *
     * @return 能力名
     */
    std::string getAbilityName() const { return m_model.abilityName; }

    /**
     * 设置描述文本。
     *
     * @param desc 描述文本
     */
    void setDescription(const std::string& desc) { m_model.description = desc; }

    /**
     * 获取描述文本。
     *
     * @return 描述文本
     */
    std::string getDescription() const { return m_model.description; }

    /**
     * 获取可写模型引用。
     *
     * @return 模型引用
     */
    CardModel& getModel() { return m_model; }

    /**
     * 获取只读模型引用。
     *
     * @return 模型只读引用
     */
    const CardModel& getModel() const { return m_model; }

private:
    CardModel m_model;
    CardView m_view;
};
