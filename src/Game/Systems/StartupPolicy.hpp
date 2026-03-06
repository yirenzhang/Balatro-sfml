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

StartupDecision Evaluate(const StartupInputs& inputs);

} // namespace StartupPolicy
