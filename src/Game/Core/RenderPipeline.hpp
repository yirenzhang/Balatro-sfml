#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "../Data/CRTParams.hpp"

class RenderPipeline {
public:
    /**
     * 初始化离屏渲染目标。
     *
     * 先离屏再上屏可以把后处理与场景绘制解耦，
     * 降低渲染流程在状态层的复杂度。
     *
     * @param width 渲染宽度
     * @param height 渲染高度
     * @return 初始化是否成功
     */
    bool init(unsigned width, unsigned height);

    /**
     * 加载 CRT 后处理着色器。
     *
     * 将着色器加载留在渲染管线内部，目的是保证渲染降级路径
     * 与正常路径在一个模块内维护。
     *
     * @param fragPath 片元着色器路径
     * @return 加载是否成功
     */
    bool loadShader(const std::string& fragPath);

    /**
     * 更新时间相关 uniform 状态。
     *
     * @param dt 帧间隔秒数
     */
    void update(float dt);

    /**
     * 开始新帧离屏绘制。
     *
     * @param clearColor 清屏颜色
     */
    void beginFrame(const sf::Color& clearColor);

    /**
     * 结束离屏绘制并提交纹理。
     */
    void endFrame();

    /**
     * 获取离屏绘制目标。
     *
     * @return 可写入的渲染目标
     */
    sf::RenderTarget& target() { return m_renderTexture; }

    /**
     * 将离屏结果绘制到窗口。
     *
     * 该接口屏蔽“有 shader / 无 shader”两条路径，
     * 以保持上层渲染调用一致。
     *
     * @param window 目标窗口
     * @param params CRT 参数
     */
    void present(sf::RenderWindow& window, const CRTParams& params);

private:
    sf::RenderTexture m_renderTexture;
    sf::Shader m_crtShader;
    bool m_shaderLoaded = false;
    float m_shaderTime = 0.0f;
};
