#pragma once

#include <memory>
#include "BasicStateMachine.hpp"
#include "../States/IGameState.hpp"

class Game;

class StateMachine {
public:
    void changeState(Game& game, std::unique_ptr<IGameState> newState) {
        m_machine.changeState(game, std::move(newState));
    }

    IGameState* currentState() const { return m_machine.currentState(); }

private:
    BasicStateMachine<Game, IGameState> m_machine;
};
