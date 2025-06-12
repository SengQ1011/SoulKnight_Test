//
// Created by tjx20 on 6/12/2025.
//

#include "UIPanel/TalentSelectionPanel.hpp"
#include "GameMechanism/TalentDatabase.hpp"
#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "RandomUtil.hpp"
#include "SaveManager.hpp"
#include "Scene/SceneManager.hpp"
#include "Tool/Tool.hpp"
#include "UIPanel/UIButton.hpp"
#include "UIPanel/UIManager.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"
#include "Util/Text.hpp"
#include "config.hpp"

TalentSelectionPanel::TalentSelectionPanel() {
    // 從 SceneManager 獲取玩家數據
    auto& sceneManager = SceneManager::GetInstance();
    auto saveData = sceneManager.DownloadGameProgress();
    if (!saveData) {
        LOG_ERROR("Failed to get save data from SceneManager");
        return;
    }

    // 獲取玩家已擁有的天賦ID
    m_PlayerTalentIDs = saveData->gameProgress.playerData.talentID;
    
    // 從天賦數據庫獲取所有可用天賦
    m_AvailableTalents = CreateTalentList();
}

void TalentSelectionPanel::Start() {
    LOG_DEBUG("TalentSelectionPanel::Start() - Begin");

    // 設置面板名稱用於輸入阻擋檢查
    SetPanelName("talent_selection");

    // 創建遮罩層 - 使用半透明黑色
    m_OverLay = std::make_shared<nGameObject>("TalentOverlay");
    auto overlayImage = ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/overlay_black.png");
    if (!overlayImage) {
        LOG_ERROR("Failed to load overlay image");
    }
    m_OverLay->SetDrawable(overlayImage);
    m_OverLay->SetZIndex(100.0f);
    // 設置遮罩層大小為屏幕大小
    m_OverLay->m_Transform.scale = glm::vec2(
        static_cast<float>(PTSD_Config::WINDOW_WIDTH),
        static_cast<float>(PTSD_Config::WINDOW_HEIGHT)
    );
    m_OverLay->SetControlVisible(true);
    m_GameObjects.push_back(m_OverLay);
    LOG_DEBUG("Created overlay: {}", m_OverLay->GetName());

    // 創建面板背景 - 使用半透明背景
    m_PanelBackground = std::make_shared<nGameObject>("TalentBackground");
    auto bgImage = ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_pausePanel/background_pausePanel.png");
    if (!bgImage) {
        LOG_ERROR("Failed to load background image");
    }
    m_PanelBackground->SetDrawable(bgImage);
    m_PanelBackground->SetZIndex(101.0f);
    // 設置背景位置在屏幕中央
    m_PanelBackground->m_Transform.translation = 
        (glm::vec2(static_cast<float>(PTSD_Config::WINDOW_WIDTH), static_cast<float>(PTSD_Config::WINDOW_HEIGHT)) -
         m_PanelBackground->GetScaledSize()) *
        glm::vec2(-0.5f, 0.5f);
    m_PanelBackground->SetControlVisible(true);
    m_GameObjects.push_back(m_PanelBackground);
    LOG_DEBUG("Created background: {}", m_PanelBackground->GetName());

    // 創建標題文字
    m_TitleText = std::make_shared<nGameObject>("TalentTitle");
	const auto titleText = ImagePoolManager::GetInstance().GetText(RESOURCE_DIR "/Font/jf-openhuninn-2.1.ttf", 1000, "選擇天賦",
                                                           Util::Color(255, 255, 255));
    if (!titleText) {
        LOG_ERROR("Failed to create title text");
    }
    m_TitleText->SetDrawable(titleText);
    m_TitleText->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	LOG_DEBUG("position: {}",m_TitleText->m_WorldCoord);
    // 將標題放在背景上方
    m_TitleText->m_Transform.translation = glm::vec2(0.0f, 100.0f);
    m_TitleText->SetControlVisible(true);
    m_GameObjects.push_back(m_TitleText);
	LOG_DEBUG("position: {}",m_TitleText->m_WorldCoord);
    LOG_DEBUG("Created title: {}", m_TitleText->GetName());

    // 初始化天賦按鈕
    InitializeTalentButtons();

    // 添加到場景
    const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
    if (scene) {
        if (const auto renderer = scene->GetRoot().lock()) {
            LOG_DEBUG("Adding UI elements to scene");
            // 確保所有元素都被添加到場景中
            for (const auto& gameObject : m_GameObjects) {
                if (gameObject) {
                    renderer->AddChild(gameObject);
                    gameObject->SetControlVisible(true);
                    LOG_DEBUG("Added to scene: {}", gameObject->GetName());
                }
            }
        } else {
            LOG_ERROR("Failed to get renderer");
        }
    } else {
        LOG_ERROR("Failed to get current scene");
    }

    // 直接顯示面板
    Show();
	scene->FlushPendingObjectsToRendererAndCamera();
    LOG_DEBUG("TalentSelectionPanel::Start() - End");
}

void TalentSelectionPanel::InitializeTalentButtons() {
    LOG_DEBUG("Initializing talent buttons");
    m_TalentButtons.clear();
    m_TalentButtons.resize(TALENT_OPTIONS);

    // 計算按鈕位置
    const float buttonSpacing = 250.0f;  // 按鈕間距
    const float totalWidth = (TALENT_OPTIONS - 1) * buttonSpacing;
    const float startX = -totalWidth / 2.0f;
    const float buttonY = 0.0f;  // 按鈕垂直位置

    // 過濾出玩家未擁有的天賦
    std::vector<Talent> availableTalents;
    for (const auto& talent : m_AvailableTalents) {
        if (std::find(m_PlayerTalentIDs.begin(), m_PlayerTalentIDs.end(), talent.GetId()) == m_PlayerTalentIDs.end()) {
            availableTalents.push_back(talent);
        }
    }

    LOG_DEBUG("Available talents count: {}", availableTalents.size());

    // 隨機選擇天賦
    std::vector<Talent> selectedTalents;
    if (availableTalents.size() <= TALENT_OPTIONS) {
        selectedTalents = availableTalents;
    } else {
        // 使用 Fisher-Yates 洗牌算法
        for (int i = 0; i < TALENT_OPTIONS; ++i) {
            int remaining = static_cast<int>(availableTalents.size()) - i;
            int randomIndex = RandomUtil::RandomIntInRange(0, remaining - 1);
            selectedTalents.push_back(availableTalents[randomIndex]);
            std::swap(availableTalents[randomIndex], availableTalents[remaining - 1]);
        }
    }

    LOG_DEBUG("Selected talents count: {}", selectedTalents.size());

    // 創建天賦按鈕
    for (int i = 0; i < TALENT_OPTIONS; ++i) {
        // 創建按鈕背景
        auto buttonBg = std::make_shared<nGameObject>("TalentButtonBg_" + std::to_string(i));
        auto bgImage = ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_talentIcon/ui_buff_00.png");
        if (!bgImage) {
            LOG_ERROR("Failed to load button background image");
        }
        buttonBg->SetDrawable(bgImage);
        buttonBg->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
        buttonBg->m_Transform.translation = glm::vec2(startX + i * buttonSpacing, buttonY);
        buttonBg->m_Transform.scale = glm::vec2(2.0f, 2.0f);
        buttonBg->SetControlVisible(true);
        m_GameObjects.push_back(buttonBg);

        // 創建按鈕
        auto button = std::make_shared<UIButton>(nullptr, false);
        button->SetZIndex(buttonBg->GetZIndex() + 0.1f);
        button->m_Transform.translation = buttonBg->m_Transform.translation;
        button->m_Transform.scale = buttonBg->m_Transform.scale;

        if (i < static_cast<int>(selectedTalents.size())) {
            const auto& talent = selectedTalents[i];
            const std::string iconPath = RESOURCE_DIR + talent.GetIconPath();
            LOG_DEBUG("Setting talent icon: {}", iconPath);
            auto image = ImagePoolManager::GetInstance().GetImage(iconPath);
            if (!image) {
                LOG_ERROR("Failed to load talent icon: {}", iconPath);
            }
            button->SetDrawable(image);

            // 創建天賦名稱文字
            auto nameText = std::make_shared<nGameObject>("TalentName_" + std::to_string(i));
            auto textDrawable = ImagePoolManager::GetInstance().GetText(
                RESOURCE_DIR "/Font/jf-openhuninn-2.1.ttf", 
                48,  // 增大字體大小
                talent.GetName(),
                Util::Color(255, 255, 255)
            );
            if (textDrawable) {
                nameText->SetDrawable(textDrawable);
                nameText->SetZIndex(button->GetZIndex() + 0.2f);  // 確保文字在最上層
                // 將文字放在按鈕下方，並調整位置
                nameText->m_Transform.translation = button->m_Transform.translation + glm::vec2(0.0f, 100.0f);
                nameText->m_Transform.scale = glm::vec2(1.0f, 1.0f);  // 設置適當的縮放
                nameText->SetControlVisible(true);
                m_GameObjects.push_back(nameText);
                LOG_DEBUG("Created talent name text: {} at position ({}, {})", 
                    talent.GetName(), 
                    nameText->m_Transform.translation.x,
                    nameText->m_Transform.translation.y);
            }

            // 設置按鈕回調
            button->SetFunction([this, talent]() {
                LOG_DEBUG("Talent selected: {}", talent.GetName());
                auto& sceneManager = SceneManager::GetInstance();
                if (auto saveData = sceneManager.DownloadGameProgress()) {
                    saveData->gameProgress.playerData.talentID.push_back(talent.GetId());
                    sceneManager.UploadGameProgress(saveData);
                }
                AudioManager::GetInstance().PlaySFX("click");
                this->Hide();
            });
        } else {
            LOG_DEBUG("Setting default icon for button {}", i);
            auto defaultImage = ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_talentIcon/ui_buff_00.png");
            if (!defaultImage) {
                LOG_ERROR("Failed to load default icon");
            }
            button->SetDrawable(defaultImage);
        }

        button->SetControlVisible(true);
        m_TalentButtons[i] = button;
        m_GameObjects.push_back(button);
        LOG_DEBUG("Created talent button: {}", button->GetName());
    }
}

void TalentSelectionPanel::Update() {
	// if (Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB)) LOG_DEBUG("click: {}",Tool::GetMouseCoord());
    // 更新所有UI元素
    for (const auto& gameObject : m_GameObjects) {
        if (gameObject) {
            gameObject->SetControlVisible(true);
            gameObject->Update();
        }
    }

    // 繪製調試UI
    DrawDebugUI();
}

void TalentSelectionPanel::Show() {
    UIPanel::Show();
    // 確保所有元素可見
    for (const auto& gameObject : m_GameObjects) {
        if (gameObject) {
            gameObject->SetControlVisible(true);
        }
    }
}

void TalentSelectionPanel::Hide() {
    UIPanel::Hide();
    // 確保所有元素隱藏
    for (const auto& gameObject : m_GameObjects) {
        if (gameObject) {
            gameObject->SetControlVisible(false);
        }
    }
}

void TalentSelectionPanel::DrawDebugUI() {
    ImGui::Begin("Talent Selection Panel Debug");

    // Panel Control
    if (ImGui::CollapsingHeader("Panel Control", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Show Panel")) {
            Show();
        }
        ImGui::SameLine();
        if (ImGui::Button("Hide Panel")) {
            Hide();
        }
        ImGui::Text("Panel Visible: %s", IsVisible() ? "Yes" : "No");
    }

    // Element Status
    if (ImGui::CollapsingHeader("Element Status", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (m_OverLay) {
            ImGui::Text("Overlay: %s", m_OverLay->GetName().c_str());
            ImGui::Text("Position: (%.1f, %.1f)", m_OverLay->m_Transform.translation.x, m_OverLay->m_Transform.translation.y);
            ImGui::Text("Scale: (%.1f, %.1f)", m_OverLay->m_Transform.scale.x, m_OverLay->m_Transform.scale.y);
            ImGui::Text("Z-Index: %.1f", m_OverLay->GetZIndex());
        }

        if (m_PanelBackground) {
            ImGui::Text("Background: %s", m_PanelBackground->GetName().c_str());
            ImGui::Text("Position: (%.1f, %.1f)", m_PanelBackground->m_Transform.translation.x, m_PanelBackground->m_Transform.translation.y);
            ImGui::Text("Scale: (%.1f, %.1f)", m_PanelBackground->m_Transform.scale.x, m_PanelBackground->m_Transform.scale.y);
            ImGui::Text("Z-Index: %.1f", m_PanelBackground->GetZIndex());
        }

        if (m_TitleText) {
            ImGui::Text("Title: %s", m_TitleText->GetName().c_str());
            ImGui::Text("Position: (%.1f, %.1f)", m_TitleText->m_Transform.translation.x, m_TitleText->m_Transform.translation.y);
            ImGui::Text("Z-Index: %.1f", m_TitleText->GetZIndex());
        }
    }

    // Talent Buttons
    if (ImGui::CollapsingHeader("Talent Buttons", ImGuiTreeNodeFlags_DefaultOpen)) {
        for (int i = 0; i < TALENT_OPTIONS; ++i) {
            if (m_TalentButtons[i]) {
                ImGui::Text("Button %d: %s", i, m_TalentButtons[i]->GetName().c_str());
                ImGui::Text("Position: (%.1f, %.1f)", m_TalentButtons[i]->m_Transform.translation.x, m_TalentButtons[i]->m_Transform.translation.y);
                ImGui::Text("Scale: (%.1f, %.1f)", m_TalentButtons[i]->m_Transform.scale.x, m_TalentButtons[i]->m_Transform.scale.y);
                ImGui::Text("Z-Index: %.1f", m_TalentButtons[i]->GetZIndex());
            }
        }
    }

    ImGui::End();
}