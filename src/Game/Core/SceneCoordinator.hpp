#pragma once

#include <memory>

#include "../Objects/CardArea.hpp"
#include "GameContext.hpp"

class SceneCoordinator {
public:
    void initDefaultLayout(GameContext& ctx, float width, float height);

    CardArea* handArea() const { return m_handArea.get(); }
    CardArea* jokerArea() const { return m_jokerArea.get(); }
    CardArea* shopArea() const { return m_shopArea.get(); }

private:
    std::shared_ptr<CardArea> m_handArea;
    std::shared_ptr<CardArea> m_jokerArea;
    std::shared_ptr<CardArea> m_shopArea;
};
