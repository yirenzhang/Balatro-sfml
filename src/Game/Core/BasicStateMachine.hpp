#pragma once

#include <memory>

template <typename Host, typename State>
class BasicStateMachine {
public:
    void changeState(Host& host, std::unique_ptr<State> newState) {
        if (m_currentState) {
            m_currentState->onExit(host);
        }

        m_currentState = std::move(newState);

        if (m_currentState) {
            m_currentState->onEnter(host);
        }
    }

    State* currentState() const { return m_currentState.get(); }

private:
    std::unique_ptr<State> m_currentState;
};
