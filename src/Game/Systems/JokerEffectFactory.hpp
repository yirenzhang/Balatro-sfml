#pragma once

#include <memory>
#include <string>
#include <nlohmann/json.hpp>

#include "../Effects/IEffect.hpp"

namespace JokerEffectFactory {

/**
 * 按配置 ID 创建效果对象。
 *
 * 工厂集中创建的原因是避免数据库模块直接依赖具体效果类，
 * 让新增效果时只修改一处分发逻辑。
 *
 * @param effectId 效果类型 ID
 * @param params 效果参数
 * @return 效果对象；未知 ID 返回 nullptr
 */
std::shared_ptr<IEffect> Create(const std::string& effectId, const nlohmann::json& params);

}
