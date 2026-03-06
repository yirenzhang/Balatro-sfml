#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "../Core/GameContext.hpp"

class UIManager {
public:
    /**
     * 构造 UI 管理器。
     */
    UIManager() = default;

    /**
     * 初始化 UI 资源与布局。
     *
     * @param font UI 字体
     */
    void init(const sf::Font& font);

    /**
     * 刷新 HUD 文本。
     *
     * @param ctx 运行上下文
     */
    void update(const GameContext& ctx);

    /**
     * 更新牌型面板信息。
     *
     * @param handName 牌型名称
     * @param level 牌型等级
     * @param chips 基础筹码
     * @param mult 基础倍率
     */
    void updateHandInfo(const std::string& handName, int level, int chips, int mult);

    /**
     * 设置商店提示文本。
     *
     * @param msg 提示内容
     * @param color 文本颜色
     */
    void setShopMessage(const std::string& msg, sf::Color color);

    /**
     * 绘制 UI。
     *
     * @param target 绘制目标
     * @param state 当前游戏状态
     */
    void draw(sf::RenderTarget& target, GameState state);

private:
    void setupText(sf::Text& t, int size, sf::Color color, sf::Vector2f pos, const sf::Font& font);
    void centerTextInBox(sf::Text& t, const sf::RectangleShape& box);

    sf::Text m_textHUD;
    sf::Text m_textScore;
    sf::Text m_textDeckCount;
    sf::Text m_textShopInfo;

    sf::RectangleShape m_handInfoBg;
    sf::RectangleShape m_chipsBox;
    sf::RectangleShape m_multBox;
    sf::Text m_textHandType;
    sf::Text m_textHandLevel;
    sf::Text m_textBaseChips;
    sf::Text m_textBaseMult;
    sf::Text m_textMultSymbol;
};
