#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "GameContext.hpp"
#include "SceneCoordinator.hpp"
#include "../Objects/Card.hpp"
#include "../UI/Tooltip.hpp"

class HoverTooltipController {
public:
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
