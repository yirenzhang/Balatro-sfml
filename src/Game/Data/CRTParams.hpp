#pragma once
#include <SFML/Graphics.hpp>

/**
 * CRT Shader 参数配置
 * * 封装了所有调节 CRT 效果的变量，方便统一管理和调试。
 */
struct CRTParams {
    // 1. 扫描线密度 (Scanlines)
    // 你的设定: 720.0f * 5.0f = 3600.0f
    // 密度越高，线条越细。
    float scanlines = 720.0f * 5.0f; 

    // 2. CRT 强度 (Intensity)
    // 你的设定: 0.1f (原版 2.0-3.0 太黑了，0.1 更清晰)
    float crtIntensity = 0.1f;

    // 3. 泛光强度 (Bloom)
    // 你的设定: 0.8f
    float bloomIntensity = 0.8f;

    // 4. 畸变因子 (Distortion)
    // 你的设定: x=1.06, y=1.065 (轻微的鱼眼效果)
    sf::Vector2f distortion = {1.06f, 1.065f};

    // 5. 缩放因子 (Scale)
    // 你的设定: 1.0 (保持原大小，配合畸变使用)
    sf::Vector2f scale = {1.0f, 1.0f};

    // 6. 边缘羽化 (Feather)
    // 你的设定: 0.02 (边缘轻微模糊)
    float feather = 0.02f;

    // 7. 噪点强度 (Noise)
    // 你的设定: 0.1 (轻微的模拟信号噪点)
    float noise = 0.1f;

    // 8. 故障强度 (Glitch)
    // 你的设定: 0.0 (默认关闭)
    float glitch = 0.0f;

    /**
     * 将参数应用到 Shader
     * * 对应 CRT.fs 中的 uniform 变量名
     * * @param shader 目标 Shader 对象
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