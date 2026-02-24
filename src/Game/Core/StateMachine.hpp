#pragma once

#include <memory>
#include "../States/IGameState.hpp"

class Game;

class StateMachine {
public:
    void changeState(Game& game, std::unique_ptr<IGameState> newState) {
        if (m_currentState) {
            m_currentState->onExit(game);
        }

        m_currentState = std::move(newState);

        if (m_currentState) {
            m_currentState->onEnter(game);
        }
    }

    IGameState* currentState() const { return m_currentState.get(); }

private:
    std::unique_ptr<IGameState> m_currentState;
};
