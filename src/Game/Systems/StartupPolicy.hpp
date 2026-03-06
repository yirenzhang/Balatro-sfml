#pragma once

#include <string>
#include <vector>

struct StartupInputs {
    bool shaderLoaded = false;
    bool deckTextureLoaded = false;
    bool jokersTextureLoaded = false;
    bool fontLoaded = false;
    bool ranksLoaded = false;
    bool jokersDbLoaded = false;
};

struct StartupDecision {
    bool canStart = false;
    bool degraded = false;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

namespace StartupPolicy {

/**
 * 评估启动可行性。
 *
 * 将“阻断启动”和“可降级运行”分层，是为了在资源缺失时
 * 明确行为边界，避免半初始化状态进入主循环。
 *
 * @param inputs 启动输入状态
 * @return 启动决策
 */
StartupDecision Evaluate(const StartupInputs& inputs);

} // namespace StartupPolicy
