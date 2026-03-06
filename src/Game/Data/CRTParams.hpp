#pragma once

#include <SFML/Graphics.hpp>

/**
 * CRT 后处理参数。
 *
 * 统一集中参数的目的是让视觉调优与渲染流程解耦，
 * 避免 shader 变量分散在多处造成维护成本上升。
 */
struct CRTParams {
    float scanlines = 720.0f * 5.0f;
    float crtIntensity = 0.1f;
    float bloomIntensity = 0.8f;
    sf::Vector2f distortion = {1.06f, 1.065f};
    sf::Vector2f scale = {1.0f, 1.0f};
    float feather = 0.02f;
    float noise = 0.1f;
    float glitch = 0.0f;

    /**
     * 将参数写入 shader uniform。
     *
     * 统一入口可避免调用方遗漏某个参数，导致不同渲染路径表现不一致。
     *
     * @param shader 目标 shader
     */
    void applyTo(sf::Shader& shader) const {
        shader.setUniform("scanlines", scanlines);
        shader.setUniform("crt_intensity", crtIntensity);
        shader.setUniform("bloom_fac", bloomIntensity);
        shader.setUniform("distortion_fac", distortion);
        shader.setUniform("scale_fac", scale);
        shader.setUniform("feather_fac", feather);
        shader.setUniform("noise_fac", noise);
        shader.setUniform("glitch_intensity", glitch);
    }
};
