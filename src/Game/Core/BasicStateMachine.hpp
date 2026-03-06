#pragma once

#include <memory>

template <typename Host, typename State>
class BasicStateMachine {
public:
    /**
     * 切换当前状态。
     *
     * 先调用旧状态 `onExit` 再调用新状态 `onEnter`，
     * 目的是保证生命周期顺序稳定，避免状态间资源交叉污染。
     *
     * @param host 状态回调所依赖的宿主对象
     * @param newState 新状态所有权
     */
    void changeState(Host& host, std::unique_ptr<State> newState) {
        if (m_currentState) {
            m_currentState->onExit(host);
        }

        m_currentState = std::move(newState);

        if (m_currentState) {
            m_currentState->onEnter(host);
        }
    }

    /**
     * 读取当前状态指针。
     *
     * 返回裸指针是为了只读访问，不转移状态所有权。
     *
     * @return 当前状态，若无状态则返回 nullptr
     */
    State* currentState() const { return m_currentState.get(); }

private:
    std::unique_ptr<State> m_currentState;
};
