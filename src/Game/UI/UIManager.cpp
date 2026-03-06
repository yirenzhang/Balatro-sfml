#include "UIManager.hpp"

void UIManager::init(const sf::Font& font) {
    setupText(m_textHUD, 30, sf::Color::White, {40.0f, 200.0f}, font);
    setupText(m_textScore, 30, sf::Color::White, {40.0f, 240.0f}, font);
    setupText(m_textDeckCount, 30, sf::Color(100, 200, 255), {1050.0f, 650.0f}, font);

    setupText(m_textShopInfo, 40, sf::Color::Yellow, {420.0f, 500.0f}, font);
    m_textShopInfo.setString("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round");

    m_handInfoBg.setSize({260.0f, 120.0f});
    m_handInfoBg.setFillColor(sf::Color(20, 20, 20, 200));
    m_handInfoBg.setPosition(30.0f, 300.0f);

    m_chipsBox.setSize({100.0f, 50.0f});
    m_chipsBox.setFillColor(sf::Color(0, 120, 255));
    m_chipsBox.setPosition(40.0f, 360.0f);

    m_multBox.setSize({100.0f, 50.0f});
    m_multBox.setFillColor(sf::Color(255, 60, 60));
    m_multBox.setPosition(170.0f, 360.0f);

    setupText(m_textHandType, 30, sf::Color::White, {40.0f, 305.0f}, font);
    setupText(m_textHandLevel, 20, sf::Color::White, {0, 0}, font);
    setupText(m_textBaseChips, 28, sf::Color::White, {0, 0}, font);
    setupText(m_textBaseMult, 28, sf::Color::White, {0, 0}, font);

    setupText(m_textMultSymbol, 24, sf::Color(255, 60, 60), {150.0f, 365.0f}, font);
    m_textMultSymbol.setString("X");

    updateHandInfo("Select Hand", 1, 0, 0);
}

void UIManager::update(const GameContext& ctx) {
    m_textHUD.setString("Hands: " + std::to_string(ctx.handsLeft) +
                        "   Discards: " + std::to_string(ctx.discardsLeft) +
                        "   $: " + std::to_string(ctx.money));

    m_textScore.setString("Score: " + std::to_string(ctx.currentScore) +
                          " / " + std::to_string(ctx.targetScore));

    m_textDeckCount.setString("Deck: " + std::to_string(ctx.deck.getRemainingCount()));
}

void UIManager::updateHandInfo(const std::string& handName, int level, int chips, int mult) {
    if (handName.empty()) {
        m_textHandType.setString("Select Hand");
        m_textHandLevel.setString("");
        m_textBaseChips.setString("-");
        m_textBaseMult.setString("-");
    } else {
        m_textHandType.setString(handName);
        m_textHandLevel.setString("Lvl." + std::to_string(level));
        m_textBaseChips.setString(std::to_string(chips));
        m_textBaseMult.setString(std::to_string(mult));
    }

    m_textHandLevel.setPosition(m_textHandType.getPosition().x + m_textHandType.getGlobalBounds().width + 15.0f, 315.0f);
    centerTextInBox(m_textBaseChips, m_chipsBox);
    centerTextInBox(m_textBaseMult, m_multBox);
}

void UIManager::setShopMessage(const std::string& msg, sf::Color color) {
    m_textShopInfo.setString(msg);
    m_textShopInfo.setFillColor(color);
}

void UIManager::draw(sf::RenderTarget& target, GameState state) {
    target.draw(m_textHUD);
    target.draw(m_textScore);
    target.draw(m_textDeckCount);

    if (state == GameState::Run) {
        target.draw(m_handInfoBg);
        target.draw(m_chipsBox);
        target.draw(m_multBox);
        target.draw(m_textHandType);
        target.draw(m_textHandLevel);
        target.draw(m_textBaseChips);
        target.draw(m_textBaseMult);
        target.draw(m_textMultSymbol);
    } else if (state == GameState::Shop) {
        target.draw(m_textShopInfo);
    }
}

void UIManager::setupText(sf::Text& t, int size, sf::Color color, sf::Vector2f pos, const sf::Font& font) {
    t.setFont(font);
    t.setCharacterSize(size);
    t.setFillColor(color);
    t.setPosition(pos);
}

void UIManager::centerTextInBox(sf::Text& t, const sf::RectangleShape& box) {
    const sf::FloatRect tb = t.getLocalBounds();
    const sf::Vector2f bp = box.getPosition();
    const sf::Vector2f bs = box.getSize();
    t.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    t.setPosition(bp.x + bs.x / 2.0f, bp.y + bs.y / 2.0f);
}
