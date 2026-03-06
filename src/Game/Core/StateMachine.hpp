#pragma once

#include <memory>
#include "BasicStateMachine.hpp"
#include "../States/IGameState.hpp"

class Game;

class StateMachine {
public:
    /**
     * 切换游戏状态。
     *
     * 使用封装器的原因是让 Game 只依赖 `IGameState`，
     * 具体切换细节由状态机内部统一维护。
     *
     * @param game 游戏宿主
     * @param newState 新状态所有权
     */
    void changeState(Game& game, std::unique_ptr<IGameState> newState) {
        m_machine.changeState(game, std::move(newState));
    }

    /**
     * 读取当前状态。
     *
     * @return 当前状态，若为空则返回 nullptr
     */
    IGameState* currentState() const { return m_machine.currentState(); }

private:
    BasicStateMachine<Game, IGameState> m_machine;
};
