#include "SceneCoordinator.hpp"

void SceneCoordinator::initDefaultLayout(GameContext& ctx, float width, float height) {
    float jokerW = 600.0f;
    float jokerH = 150.0f;
    m_jokerArea = std::make_shared<CardArea>((width - jokerW) / 2.0f, 30.0f, jokerW, jokerH, LayoutType::Row);
    ctx.area_jokers = m_jokerArea.get();

    float shopW = 600.0f;
    float shopH = 150.0f;
    m_shopArea = std::make_shared<CardArea>((width - shopW) / 2.0f, 300.0f, shopW, shopH, LayoutType::Row);
    ctx.area_shop = m_shopArea.get();

    float handW = 800.0f;
    float handH = 280.0f;
    m_handArea = std::make_shared<CardArea>(
        (width - handW) / 2.0f,
        height - handH - 30.0f,
        handW,
        handH,
        LayoutType::Fan
    );
    ctx.area_hand = m_handArea.get();
}
