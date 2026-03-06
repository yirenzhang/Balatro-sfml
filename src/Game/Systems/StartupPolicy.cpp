#include "StartupPolicy.hpp"

StartupDecision StartupPolicy::Evaluate(const StartupInputs& inputs) {
    StartupDecision d;

    if (!inputs.deckTextureLoaded) d.errors.push_back("Missing critical texture: deck");
    if (!inputs.jokersTextureLoaded) d.errors.push_back("Missing critical texture: jokers");
    if (!inputs.fontLoaded) d.errors.push_back("Missing critical font: main");
    if (!inputs.ranksLoaded) d.errors.push_back("Missing critical data: ranks");
    if (!inputs.jokersDbLoaded) d.errors.push_back("Missing critical data: jokers database");

    if (!inputs.shaderLoaded) {
        d.warnings.push_back("CRT shader unavailable, fallback to non-shader render path.");
    }

    d.canStart = d.errors.empty();
    d.degraded = d.canStart && !d.warnings.empty();
    return d;
}
