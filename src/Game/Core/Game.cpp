#include "Game.hpp"
#include "../Systems/HandEvaluator.hpp"
#include "../Systems/GameDatabase.hpp"

#include <iostream>
#include <ctime>
#include <cstdlib>

Game::Game() {
    // 0. 初始化随机种子
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    initWindow();
    initResources();
    initScene();
}

void Game::initWindow() {
    m_window.create(sf::VideoMode(1280, 720), "Balatro C++ Rewrite (Refactored)");
    m_window.setFramerateLimit(60);

    // 初始化虚拟画布
    if (!m_renderTexture.create(1280, 720)) {
        std::cerr << "Error: Failed to create render texture!" << std::endl;
    }
}

void Game::initResources() {
    // 加载 Shader
    if (m_crtShader.loadFromFile("assets/shaders/CRT.fs", sf::Shader::Fragment)) {
        m_shaderLoaded = true;
        
        m_crtShader.setUniform("texture", sf::Shader::CurrentTexture);
        m_crtShader.setUniform("resolution", sf::Vector2f(1280, 720));
        
        // 参数配置
        m_crtShader.setUniform("scanlines", 720.0f * 5.0f); 
        m_crtShader.setUniform("crt_intensity", 0.1f);
        m_crtShader.setUniform("bloom_fac", 0.4f);
        m_crtShader.setUniform("distortion_fac", sf::Vector2f(1.06f, 1.065f));
        m_crtShader.setUniform("scale_fac", sf::Vector2f(1.0f, 1.0f));
        m_crtShader.setUniform("feather_fac", 0.02f);
        m_crtShader.setUniform("noise_fac", 0.1f);
        m_crtShader.setUniform("glitch_intensity", 0.0f);
    } else {
        std::cerr << "Warning: Failed to load CRT Shader." << std::endl;
    }

    // 加载纹理
    if (!m_textureDeck.loadFromFile("assets/textures/1x/8BitDeck.png")) {
        // Fallback
        if (!m_textureDeck.loadFromFile("assets/textures/1x/Jokers.png")) {
             std::cerr << "Error: Could not load card textures!" << std::endl;
        }
    }
    m_textureDeck.setSmooth(false);

    if (!m_textureJokers.loadFromFile("assets/textures/1x/Jokers.png")) {
        std::cerr << "Error: Could not load Joker textures!" << std::endl;
    }
    m_textureJokers.setSmooth(false);

    // 加载数据库
    GameDatabase::Instance().loadRanks("assets/data/ranks.json");
    GameDatabase::Instance().loadJokers("assets/data/jokers.json");

    // 加载字体
    if (!m_font.loadFromFile("assets/fonts/m6x11plus.ttf")) {
        // 如果找不到 font.ttf，尝试加载系统字体 (Windows 示例)
        if (!m_font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cerr << "Error: Could not load font!" << std::endl;
        }
    }
    // 初始化 Tooltip
    m_tooltip.init(m_font);

    // 初始化 HUD 文本
    m_textHUD.setFont(m_font);
    m_textHUD.setCharacterSize(30);
    m_textHUD.setFillColor(sf::Color::White);
    m_textHUD.setPosition(40.0f, 200.0f); // 左上角
    m_textScore.setFont(m_font);
    m_textScore.setCharacterSize(30);
    m_textScore.setFillColor(sf::Color::White);
    m_textScore.setPosition(40.0f, 240.0f); // HUD 下方

    // 初始化 Deck UI
    m_textDeckCount.setFont(m_font);
    m_textDeckCount.setCharacterSize(30);
    m_textDeckCount.setFillColor(sf::Color(100, 200, 255)); // 淡蓝色
    m_textDeckCount.setPosition(1050.0f, 650.0f); // 右下角区域

    // 初始化商店 UI
    m_textShopInfo.setFont(m_font);
    m_textShopInfo.setCharacterSize(40);
    m_textShopInfo.setFillColor(sf::Color::Yellow);
    m_textShopInfo.setPosition(420.0f, 500.0f);
    m_textShopInfo.setString("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round");
    
    // 初始化牌型信息面板组件
    // 配置背景框 (左侧)
    m_handInfoBg.setSize({260.0f, 120.0f});
    m_handInfoBg.setFillColor(sf::Color(20, 20, 20, 200));
    m_handInfoBg.setPosition(30.0f, 300.0f);
    // 筹码框 (蓝色)
    m_chipsBox.setSize({100.0f, 50.0f});
    m_chipsBox.setFillColor(sf::Color(0, 120, 255));
    m_chipsBox.setPosition(40.0f, 360.0f);
    // 倍率框 (红色)
    m_multBox.setSize({100.0f, 50.0f});
    m_multBox.setFillColor(sf::Color(255, 60, 60));
    m_multBox.setPosition(170.0f, 360.0f);
    // 文字初始化
    auto setupText = [&](sf::Text& t, int size, sf::Color c) {
        t.setFont(m_font);
        t.setCharacterSize(size);
        t.setFillColor(c);
    };
    setupText(m_textHandType, 30, sf::Color::White);
    setupText(m_textHandLevel, 20, sf::Color::White);
    setupText(m_textBaseChips, 28, sf::Color::White);
    setupText(m_textBaseMult, 28, sf::Color::White);
    setupText(m_textMultSymbol, 24, sf::Color(255, 60, 60)); // 红色 X
    m_textMultSymbol.setString("X");
}

void Game::initScene() {
    // 初始状态：Run
    m_ctx.state = GameState::Run;

    // 初始化 Joker 区域 (顶部) - 初始为空
    float jokerW = 600.0f;
    float jokerH = 150.0f; 
    
    m_jokerArea = std::make_shared<CardArea>(
        (1280.0f - jokerW) / 2.0f, 
        30.0f, 
        jokerW, jokerH, 
        LayoutType::Row 
    );
    m_ctx.area_jokers = m_jokerArea.get();

    // 初始化 商店 区域 (中部)
    float shopW = 600.0f;
    float shopH = 150.0f;
    m_shopArea = std::make_shared<CardArea>(
        (1280.0f - shopW) / 2.0f, 
        300.0f, // 放在屏幕中间
        shopW, shopH, 
        LayoutType::Row
    );
    m_ctx.area_shop = m_shopArea.get();

    // 初始进货
    restockShop();

    // 初始化 手牌 区域 (底部)

    float handW = 800.0f;
    float handH = 280.0f;
    
    m_handArea = std::make_shared<CardArea>(
        (1280.0f - handW) / 2.0f, 
        720.0f - handH - 30.0f, 
        handW, handH, 
        LayoutType::Fan 
    );
    m_ctx.area_hand = m_handArea.get();

    // 此时 JSON 已加载，手动初始化牌堆
    m_ctx.deck.initStandardDeck();

    // 初始洗牌
    m_ctx.deck.shuffle();

    // 补满手牌
    refillHand();
}

void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }

        // =========================================================
        // 键盘交互 (Keyboard Events)
        // =========================================================
        if (event.type == sf::Event::KeyPressed) {
            
            // [测试功能] 按 M 键作弊加钱
            if (event.key.code == sf::Keyboard::M) {
                m_ctx.money += 100;
                std::cout << "[Cheat] Money +100. Current: $" << m_ctx.money << std::endl;
            }

            // 状态：RUN (打牌阶段)
            if (m_ctx.state == GameState::Run) {
                
                // 1. 弃牌 (按 D 键)
                if (event.key.code == sf::Keyboard::D) {
                    auto selected = m_handArea->getSelectedCards();
                    // 条件：有选中牌 且 还有弃牌次数
                    if (!selected.empty() && m_ctx.discardsLeft > 0) {
                        m_ctx.discardsLeft--;
                        // 销毁选中的牌
                        auto& cards = m_handArea->getCards();
                        for (int i = cards.size() - 1; i >= 0; --i) {
                            if (cards[i]->isSelected()) {
                                m_handArea->removeCard(i);
                            }
                        }
                        // 补满手牌
                        refillHand();
                        std::cout << "[Action] Discard used. Left: " << m_ctx.discardsLeft << std::endl;
                    }
                }

                // 2. 出牌 (按 Enter 键)
                if (event.key.code == sf::Keyboard::Enter) {
                    auto selected = m_handArea->getSelectedCards();
                    
                    // 条件：有选中牌 且 还有出牌次数
                    if (!selected.empty() && m_ctx.handsLeft > 0) {
                        // a. 计算得分
                        HandResult handRes = HandEvaluator::Evaluate(selected);
                        ScoreSummary summary = ScoringManager::CalculateFinalScore(handRes, m_ctx.area_jokers);

                        // [新增] 视觉反馈：为每张打出的牌生成飘字
                        for (Card* card : selected) {
                            // 1. 获取这张牌的位置
                            sf::Vector2f pos = card->getPosition();
                            // 让字稍微高一点，显示在卡牌顶部
                            pos.y -= 180.0f; 

                            // 2. 获取这张牌的基础筹码 (你是要显示 +10 还是 +11)
                            int chips = card->getChips();
                            std::string text = "+" + std::to_string(chips);

                            // 3. 生成蓝色飘字 (筹码通常是蓝色)
                            spawnFloatingText(text, pos, sf::Color(100, 150, 255));
                        }

                        // [新增] 视觉反馈：显示牌型的名称和总分 (在屏幕中央或手牌上方)
                        // 比如显示 "PAIR"
                        spawnFloatingText(handRes.name, sf::Vector2f(640, 300), sf::Color::White);

                        // b. 更新游戏数据
                        m_ctx.currentScore += summary.final_score;
                        m_ctx.handsLeft--;
                        // c. 输出详细日志
                        std::cout << "\n=== SCORING: " << handRes.name << " ===" << std::endl;
                        for(auto& s : summary.trigger_log) {
                            std::cout << " > " << s << std::endl;
                        }
                        std::cout << "SCORE: " << summary.final_score << " | TOTAL: " << m_ctx.currentScore << " / " << m_ctx.targetScore << std::endl;
                        // d. 销毁打出的牌
                        auto& cards = m_handArea->getCards();
                        for (int i = cards.size() - 1; i >= 0; --i) {
                            if (cards[i]->isSelected()) {
                                m_handArea->removeCard(i);
                            }
                        }
                        // e. 补牌
                        refillHand();
                        // f. 胜负判定
                        if (m_ctx.currentScore >= m_ctx.targetScore) {
                            std::cout << ">>> ROUND CLEARED! Entering Shop... <<<" << std::endl;
                            // 切换到商店状态
                            m_ctx.state = GameState::Shop;
                            // 奖励金钱
                            m_ctx.money += 5;
                            std::cout << "Reward: $5. Current Money: $" << m_ctx.money << std::endl;
                            // 商店进货
                            restockShop();
                        }
                        else if (m_ctx.handsLeft <= 0) {
                            std::cout << ">>> GAME OVER <<<" << std::endl;
                            m_ctx.state = GameState::GameOver;
                        }
                    }
                }
            }
            // -----------------------------------------------------
            // 状态：SHOP (商店阶段)
            // -----------------------------------------------------
            else if (m_ctx.state == GameState::Shop) {
                // 下一轮 (按 N 键)
                if (event.key.code == sf::Keyboard::N) {
                    std::cout << "Starting next round..." << std::endl;
                    startRound(); 
                    // 清空待购买状态
                    m_pendingPurchase = nullptr;
                    m_textShopInfo.setString("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round");
                    m_textShopInfo.setFillColor(sf::Color::Yellow);
                }
                // 刷新商店 (按 R 键)
                if (event.key.code == sf::Keyboard::R) {
                    int rerollCost = 5;
                    if (m_ctx.money >= rerollCost) {
                        m_ctx.money -= rerollCost;
                        restockShop();
                        m_pendingPurchase = nullptr; // 刷新也要重置选中状态
                        std::cout << "Shop rerolled! Money left: $" << m_ctx.money << std::endl;
                    } else {
                        std::cout << "Not enough money to reroll ($" << rerollCost << " needed)." << std::endl;
                    }
                }
            }
        }

        // 鼠标交互 (Mouse Events)
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Right) {
                // --- 右键取消替换操作 ---
                if (m_pendingPurchase) {
                    m_pendingPurchase->setColor(sf::Color::White);
                    m_pendingPurchase = nullptr;
                    std::cout << "Purchase cancelled." << std::endl;
                    m_textShopInfo.setString("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round");
                    m_textShopInfo.setFillColor(sf::Color::Yellow);
                }
            }

            if (event.mouseButton.button == sf::Mouse::Left) {
                // 获取鼠标世界坐标
                sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
                
                // --- RUN 状态交互 ---
                if (m_ctx.state == GameState::Run) {
                    auto clickedCard = m_handArea->getCardAt(mousePos.x, mousePos.y);
                    if (clickedCard) {
                        // [Fix] 限制选中数量不超过 5 张
                        if (clickedCard->isSelected()) {
                            // 如果已经选中，总是允许取消选中
                            clickedCard->toggleSelect();
                        } else {
                            // 如果未选中，检查当前数量
                            if (m_handArea->getSelectedCards().size() < 5) {
                                clickedCard->toggleSelect();
                            } else {
                                std::cout << "Max selection reached (5 cards)!" << std::endl;
                            }
                        }
                    }
                }
                
                // --- SHOP 状态交互 ---
                else if (m_ctx.state == GameState::Shop) {
                    
                    // 1. 检测是否点击了 Joker 区 (用于替换)
                    auto clickedJoker = m_jokerArea->getCardAt(mousePos.x, mousePos.y);

                    // 如果正在等待替换，且点了旧 Joker
                    if (m_pendingPurchase && clickedJoker) {
                        int cost = m_pendingPurchase->getCost();
                        if (m_ctx.money >= cost) {
                            m_ctx.money -= cost;
                            m_jokerArea->removeCard(clickedJoker.get()); // 卖旧
                            
                            if (m_shopArea->removeCard(m_pendingPurchase.get())) { // 拿新
                                m_pendingPurchase->setColor(sf::Color::White);
                                m_jokerArea->addCard(m_pendingPurchase);
                                std::cout << "Replaced Joker!" << std::endl;
                            }
                            
                            m_jokerArea->alignCards();
                            m_shopArea->alignCards();
                            m_pendingPurchase = nullptr;
                            m_textShopInfo.setString("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round");
                            m_textShopInfo.setFillColor(sf::Color::Yellow);
                        }
                        return;
                    }

                    // 2. 检测是否点击了 商店 区 (用于购买)
                    auto clickedShopCard = m_shopArea->getCardAt(mousePos.x, mousePos.y);
                    
                    if (clickedShopCard) {
                        int cost = clickedShopCard->getCost();
                        int jokerCount = (int)m_jokerArea->getCards().size();
                        int jokerLimit = 5;

                        // 钱不够
                        if (m_ctx.money < cost) {
                            std::cout << "Not enough money!" << std::endl;
                            return; 
                        }

                        // 情况 A: 直接购买
                        if (jokerCount < jokerLimit) {
                            m_ctx.money -= cost;
                            if (m_shopArea->removeCard(clickedShopCard.get())) {
                                clickedShopCard->setColor(sf::Color::White);
                                m_jokerArea->addCard(clickedShopCard);
                                m_shopArea->alignCards();
                                m_jokerArea->alignCards();
                                std::cout << "Bought " << clickedShopCard->getAbilityName() << std::endl;
                            }
                            // 如果之前有 pending，清空
                            if (m_pendingPurchase) {
                                // 如果之前有选中的闪烁卡，取消选中时也要重置它的颜色
                                m_pendingPurchase->setColor(sf::Color::White);
                                m_pendingPurchase = nullptr;
                                m_textShopInfo.setString("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round");
                                m_textShopInfo.setFillColor(sf::Color::Yellow);
                            }
                        }
                        // 情况 B: 槽位已满 -> 触发替换模式
                        else {
                            // 如果之前已经有一个在闪烁了，先把那个旧的重置为白色
                            if (m_pendingPurchase) {
                                m_pendingPurchase->setColor(sf::Color::White);
                            }
                            m_pendingPurchase = clickedShopCard;
                            std::cout << "Slots full! Select a Joker to replace." << std::endl;
                            m_textShopInfo.setString("SELECT A JOKER TO REPLACE\n[Right Click] Cancel");
                            m_textShopInfo.setFillColor(sf::Color::Red);
                        }
                    }
                }
            }
        }
    }
}

void Game::update(float dt) {
    // Shader 时间更新
    if (m_shaderLoaded) {
        m_shaderTime += dt;
        m_crtShader.setUniform("time", m_shaderTime);
    }

    // 获取鼠标位置
    sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
    
    // 鼠标悬停检测逻辑
    std::shared_ptr<Card> currentHovered = nullptr;
    
    // 始终检测 Joker 区域 (Run 和 Shop 都能看 Joker)
    if (m_jokerArea) {
        currentHovered = m_jokerArea->getCardAt(mousePos.x, mousePos.y);
    }

    // 根据状态检测其他区域
    if (!currentHovered) {
        if (m_ctx.state == GameState::Run) {
            // 打牌时检测手牌
            if (m_handArea) currentHovered = m_handArea->getCardAt(mousePos.x, mousePos.y);
        }
        else if (m_ctx.state == GameState::Shop) {
            // 商店时检测商品
            if (m_shopArea) currentHovered = m_shopArea->getCardAt(mousePos.x, mousePos.y);
        }
    }

    // 状态切换 (处理高亮边框等)
    if (currentHovered != m_hoveredCard) {
        if (m_hoveredCard) m_hoveredCard->setHover(false);
        if (currentHovered) currentHovered->setHover(true);
        m_hoveredCard = currentHovered;
    }

    // 物理/动画更新
    if (m_handArea) m_handArea->update(dt);
    if (m_jokerArea) m_jokerArea->update(dt);
    if (m_shopArea) m_shopArea->update(dt); 

    // 更新飘字效果
    for (auto& effect : m_effects) {
        effect.update(dt);
    }
    // 清理已经消失的飘字 (remove_if idiom)
    m_effects.erase(
        std::remove_if(m_effects.begin(), m_effects.end(), 
            [](const FloatingText& ft){ return ft.isDead(); }),
        m_effects.end()
    );

    // 商店替换模式下的呼吸灯效果
    if (m_ctx.state == GameState::Shop && m_shopArea) {
        // 如果有待购买的卡 (pendingPurchase)，让它闪烁红色
        if (m_pendingPurchase) {
            // 简单的呼吸灯效果: 红色闪烁
            float flash = (std::sin(m_shaderTime * 10.0f) + 1.0f) * 0.5f; 
            sf::Uint8 gb = 100 + (sf::Uint8)(flash * 155); 
            m_pendingPurchase->setColor(sf::Color(255, gb, gb)); 
        } 
        
        // 防止之前的闪烁状态残留
        for (auto& card : m_shopArea->getCards()) {
            if (card != m_pendingPurchase) {
                card->setColor(sf::Color::White);
            }
        }
    }

    // Tooltip 数据更新
    m_showTooltip = false;
    if (m_hoveredCard) {
        m_showTooltip = true;
        
        if (m_hoveredCard->getType() == CardType::Joker) {
            m_tooltip.update(
                m_hoveredCard->getAbilityName(),
                m_hoveredCard->getDescription(),
                mousePos
            );
        }
        else {
            // 普通牌显示筹码
            std::string desc = "Chips: " + std::to_string(m_hoveredCard->getChips());
            m_tooltip.update("Playing Card", desc, mousePos);
        }
    }

    // HUD 文本更新
    std::string hudStr = "Hands: " + std::to_string(m_ctx.handsLeft) + 
                         "   Discards: " + std::to_string(m_ctx.discardsLeft) + 
                         "   Money: $" + std::to_string(m_ctx.money);
    m_textHUD.setString(hudStr);
    std::string scoreStr = "Score: " + std::to_string(m_ctx.currentScore) + 
                           " / " + std::to_string(m_ctx.targetScore);
    m_textScore.setString(scoreStr);
    m_textDeckCount.setString("Deck: " + std::to_string(m_ctx.deck.getRemainingCount()));


    if (m_ctx.state == GameState::Run) {
        auto selected = m_handArea->getSelectedCards();
        if (!selected.empty()) {
            // 调用评估器获取当前选中的牌型数据
            HandResult res = HandEvaluator::Evaluate(selected);
            // 更新 UI 文本内容 (注意使用小写成员名)
            m_textHandType.setString(res.name);
            m_textHandLevel.setString("Level 1"); 
            m_textBaseChips.setString(std::to_string(res.base_chips));
            m_textBaseMult.setString(std::to_string(res.base_mult));

            // 文字居中处理
            auto centerText = [](sf::Text& t, sf::RectangleShape& box) {
                sf::FloatRect tb = t.getLocalBounds();
                sf::Vector2f bp = box.getPosition();
                sf::Vector2f bs = box.getSize();
                t.setOrigin(tb.left + tb.width/2.0f, tb.top + tb.height/2.0f);
                t.setPosition(bp.x + bs.x/2.0f, bp.y + bs.y/2.0f);
            };

            centerText(m_textBaseChips, m_chipsBox);
            centerText(m_textBaseMult, m_multBox);
            
            // 牌型名称排列在上方
            m_textHandType.setPosition(40.0f, 305.0f);
            m_textHandLevel.setPosition(m_textHandType.getPosition().x + m_textHandType.getGlobalBounds().width + 10.0f, 315.0f);
            
            // 乘号居中在两个框之间
            m_textMultSymbol.setPosition(150.0f, 365.0f);
        } else {
            // 没选牌时的默认显示
            m_textHandType.setString("Select Hand");
            m_textHandLevel.setString("");
            m_textBaseChips.setString("-");
            m_textBaseMult.setString("-");
        }
    }
}

void Game::render() {
    // 绘制游戏世界对象到渲染纹理（该部分会受CRT滤镜效果影响）
    m_renderTexture.clear(sf::Color(35, 35, 40));
    // 绘制小丑牌区域：若小丑牌区域对象存在，则渲染到渲染纹理
    if (m_jokerArea) m_jokerArea->draw(m_renderTexture);

    // 根据当前游戏状态，绘制对应核心区域
    if (m_ctx.state == GameState::Run) {
        if (m_handArea) m_handArea->draw(m_renderTexture);
    }
    else if (m_ctx.state == GameState::Shop) {
        if (m_shopArea) m_shopArea->draw(m_renderTexture);
    }

    // 绘制漂浮文字特效
    for (auto& effect : m_effects) {
        effect.draw(m_renderTexture);
    }

    // 结束渲染纹理的绘制，将所有绘制内容缓存为纹理对象（供后续Shader使用）
    m_renderTexture.display();
    // 准备主窗口并应用CRT滤镜Shader
    // 清空主游戏窗口（与渲染纹理区分，主窗口用于最终显示）
    m_window.clear();
    sf::Sprite canvas(m_renderTexture.getTexture());
    if (m_shaderLoaded) m_window.draw(canvas, &m_crtShader);
    else m_window.draw(canvas);

    // 绘制牌型相关文字信息
    if (m_ctx.state == GameState::Run) {
        m_window.draw(m_handInfoBg);
        m_window.draw(m_chipsBox);
        m_window.draw(m_multBox);
        
        m_window.draw(m_textHandType);
        m_window.draw(m_textHandLevel);
        m_window.draw(m_textBaseChips);
        m_window.draw(m_textBaseMult);
        m_window.draw(m_textMultSymbol);
    }

    std::string hudStr = "Hands: " + std::to_string(m_ctx.handsLeft) + 
                         "   Discards: " + std::to_string(m_ctx.discardsLeft) + 
                         "   $$$: " + std::to_string(m_ctx.money);
    m_textHUD.setString(hudStr);
    
    m_window.draw(m_textHUD);
    m_window.draw(m_textScore);
    m_window.draw(m_textDeckCount);

    if (m_ctx.state == GameState::Shop) {
        m_window.draw(m_textShopInfo);
    }

    if (m_showTooltip) m_tooltip.draw(m_window);

    m_window.display();
}

void Game::refillHand() {
    if (!m_handArea) return;

    int currentCount = (int)m_handArea->getCards().size();
    int needed = GameContext::HAND_SIZE_LIMIT - currentCount;

    for (int i = 0; i < needed; ++i) {
        auto cardDataOpt = m_ctx.deck.draw();
        
        if (!cardDataOpt.has_value()) {
            std::cout << "[Info] Deck is empty! Cannot draw more cards." << std::endl;
            break;
        }

        CardData data = cardDataOpt.value();
        auto card = std::make_shared<Card>(data.suit, data.rank, m_textureDeck);
        card->setChips(data.baseChips); // 这里已经正确调用了 setChips
        card->setInstantPosition(1100.0f, 700.0f);
        
        m_handArea->addCard(card);
    }
    m_handArea->alignCards();
}

void Game::restockShop() {
    if (!m_shopArea) return;
    
    while (!m_shopArea->getCards().empty()) {
        m_shopArea->removeCard(0);
    }

    std::vector<std::string> jokerPool = {"j_joker", "j_greedy_joker", "j_abstract"};
    
    for (int i = 0; i < 3; ++i) {
        std::string id = jokerPool[std::rand() % jokerPool.size()];
        
        if (auto card = GameDatabase::Instance().createJoker(id, m_textureJokers)) {
            m_shopArea->addCard(card);
        }
    }
    m_shopArea->alignCards();
}

void Game::startRound() {
    m_ctx.state = GameState::Run;
    
    m_ctx.currentScore = 0;
    m_ctx.handsLeft = 4;
    m_ctx.discardsLeft = 3;
    m_ctx.targetScore = (long long)(m_ctx.targetScore * 1.5f);

    while(!m_handArea->getCards().empty()) {
        m_handArea->removeCard(0);
    }

    m_ctx.deck.initStandardDeck(); 
    m_ctx.deck.shuffle();

    refillHand();
}

void Game::spawnFloatingText(const std::string& text, sf::Vector2f pos, sf::Color color) {
    m_effects.emplace_back(text, pos, color, m_font);
}