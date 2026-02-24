#pragma once

#include <memory>
#include <string>
#include <nlohmann/json.hpp>

#include "../Effects/IEffect.hpp"

namespace JokerEffectFactory {

std::shared_ptr<IEffect> Create(const std::string& effectId, const nlohmann::json& params);

}
