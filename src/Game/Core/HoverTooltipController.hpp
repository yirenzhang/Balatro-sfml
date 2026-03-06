#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "GameContext.hpp"
#include "SceneCoordinator.hpp"
#include "../Objects/Card.hpp"
#include "../UI/Tooltip.hpp"

class HoverTooltipController {
public:
    /**
     * 更新悬停状态与提示框。
     *
     * 将悬停逻辑独立封装是为了把 Game 的每帧职责收敛到编排层，
     * 避免交互细节继续膨胀。
     *
     * @param window 窗口对象
     * @param scene 场景协调器
     * @param ctx 游戏上下文
     * @param tooltip 提示框对象
     */
    void update(sf::RenderWindow& window, SceneCoordinator& scene, const GameContext& ctx, Tooltip& tooltip) {
        const sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        auto currentHovered = resolveHoveredCard(scene, ctx, mousePos);

        if (currentHovered != m_hoveredCard) {
            if (m_hoveredCard) m_hoveredCard->setHover(false);
            if (currentHovered) currentHovered->setHover(true);
            m_hoveredCard = currentHovered;
        }

        m_showTooltip = false;
        if (!m_hoveredCard) return;

        m_showTooltip = true;
        if (m_hoveredCard->getType() == CardType::Joker) {
            tooltip.update(m_hoveredCard->getAbilityName(), m_hoveredCard->getDescription(), mousePos);
        } else {
            tooltip.update("Playing Card", "Chips: " + std::to_string(m_hoveredCard->getChips()), mousePos);
        }
    }

    /**
     * 是否应显示提示框。
     *
     * @return 是否显示
     */
    bool showTooltip() const { return m_showTooltip; }

private:
    std::shared_ptr<Card> resolveHoveredCard(SceneCoordinator& scene, const GameContext& ctx, sf::Vector2f mousePos) {
        std::shared_ptr<Card> hovered = nullptr;
        if (auto* jokerArea = scene.jokerArea()) {
            hovered = jokerArea->getCardAt(mousePos.x, mousePos.y);
        }

        if (hovered) return hovered;

        if (ctx.state == GameState::Run && scene.handArea()) {
            return scene.handArea()->getCardAt(mousePos.x, mousePos.y);
        }

        if (ctx.state == GameState::Shop && scene.shopArea()) {
            return scene.shopArea()->getCardAt(mousePos.x, mousePos.y);
        }

        return nullptr;
    }

    std::shared_ptr<Card> m_hoveredCard = nullptr;
    bool m_showTooltip = false;
};
