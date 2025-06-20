//
// Created by QuzzS on 2025/3/4.
//

#include "UIPanel/PausePanel.hpp"

#include "Components/TalentComponet.hpp"
#include "GameMechanism/Talent.hpp"
#include "ImagePoolManager.hpp"
#include "Override/nGameObject.hpp"
#include "Scene/SceneManager.hpp"
#include "UIPanel/SettingPanel.hpp"
#include "UIPanel/UIButton.hpp"
#include "UIPanel/UIManager.hpp"
#include "Util/GameObject.hpp"
#include "Util/Time.hpp"
#include "config.hpp"


void PausePanel::Start()
{
	// 設置面板名稱用於輸入阻擋檢查
	SetPanelName("pause");

	// 創建遮罩層
	m_OverLay = std::make_shared<nGameObject>();
	m_OverLay->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/overlay_black.png"));
	m_OverLay->SetZIndex(90.0f); // background ZIndex (90.0f) - 0.1f
	m_GameObjects.push_back(m_OverLay);

	// 創建面板背景
	m_PanelBackground = std::make_shared<nGameObject>();
	m_PanelBackground->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_pausePanel/background_pausePanel.png"));
	m_PanelBackground->SetZIndex(m_OverLay->GetZIndex() + 0.1f);
	m_GameObjects.push_back(m_PanelBackground);

	// 計算隱藏位置（螢幕上方外側）
	const float windowHeight = static_cast<float>(PTSD_Config::WINDOW_HEIGHT);
	const float panelHeight = m_PanelBackground->GetScaledSize().y;
	m_HiddenPosition = glm::vec2(0.0f, windowHeight * 0.5f + panelHeight);
	m_VisiblePosition = m_BackgroundOffset;

	// 創建 ResumeButton - 關閉PausePanel返回游戲
	std::function<void()> resume_function = [this]() { this->Hide(); };
	m_ResumeButton = std::make_shared<UIButton>(resume_function, false);
	m_ResumeButton->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_pausePanel/button_resume.png"));
	m_ResumeButton->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_GameObjects.push_back(m_ResumeButton);

	// 創建 MenuButton - 返回主選單
	std::function<void()> menu_function = []()
	{
		// 直接設定下一個場景為主選單
		SceneManager::GetInstance().SetNextScene(Scene::SceneType::Menu);
	};
	m_MenuButton = std::make_shared<UIButton>(menu_function, false);
	m_MenuButton->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_pausePanel/button_menu.png"));
	m_MenuButton->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_GameObjects.push_back(m_MenuButton);

	// 創建 SettingButton - 呼叫SettingPanel
	std::function<void()> setting_function = [this]()
	{
		// 顯示設定面板
		UIManager::GetInstance().ShowPanel("setting");
	};
	m_SettingButton = std::make_shared<UIButton>(setting_function, false);
	m_SettingButton->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_pausePanel/button_setting.png"));
	m_SettingButton->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_GameObjects.push_back(m_SettingButton);

	// 初始化天賦顯示
	InitializeTalentIcons();

	// 將所有UI元素添加到場景渲染器
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		const auto renderer = scene->GetRoot().lock();
		renderer->AddChild(m_OverLay);
		renderer->AddChild(m_PanelBackground);
		renderer->AddChild(m_ResumeButton);
		renderer->AddChild(m_MenuButton);
		renderer->AddChild(m_SettingButton);

		// 添加天賦圖標到渲染器
		for (const auto &talentIcon : m_TalentIcons)
		{
			renderer->AddChild(talentIcon);
		}
	}

	// 設定初始位置為隱藏狀態
	UpdateAllElementsPosition(m_HiddenPosition);

	// 默認隱藏面板
	UIPanel::Hide();
}

void PausePanel::Update()
{
	// 優先更新動畫（動畫總是需要更新）
	UpdateAnimation();

	// 更新天賦顯示（顯示總是需要更新）
	UpdateTalentIcons();

	// 檢查是否應該阻擋輸入
	bool blockInput = ShouldBlockInput();

	// 更新所有元件
	// DrawDebugUI();
	for (const std::shared_ptr<nGameObject> &gameObject : m_GameObjects)
	{
		// 如果是按鈕類型且輸入被阻擋，則暫時設為不活躍
		if (auto button = std::dynamic_pointer_cast<UIButton>(gameObject))
		{
			if (blockInput)
			{
				button->SetActive(false); // 暫時禁用按鈕輸入
				button->Update(); // 仍然更新渲染
				button->SetActive(true); // 恢復活躍狀態
			}
			else
			{
				button->Update(); // 正常更新
			}
		}
		else
		{
			gameObject->Update(); // 非按鈕元件正常更新
		}
	}

	// 更新天賦圖標（只需要渲染更新）
	for (const auto &talentIcon : m_TalentIcons)
	{
		talentIcon->Update();
	}
}

void PausePanel::InitializeTalentIcons()
{
	// 清空現有的天賦圖標
	m_TalentIcons.clear();
	m_TalentIcons.resize(TALENT_SLOTS);

	for (int i = 0; i < TALENT_SLOTS; ++i)
	{
		auto talentIcon = std::make_shared<nGameObject>();

		// 設定預設圖標（空槽位）
		talentIcon->SetDrawable(
			ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_talentIcon/ui_buff_00.png"));

		talentIcon->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);

		// 設定圖標放大一倍
		talentIcon->m_Transform.scale = glm::vec2(2.0f, 2.0f);

		m_TalentIcons[i] = talentIcon;
		m_GameObjects.push_back(talentIcon);
	}
}

void PausePanel::UpdateTalentIcons()
{
	const auto talentComp = m_PlayerTalentComponent.lock();
	if (!talentComp)
		return;

	const auto &talents = talentComp->GetTalents();

	for (int i = 0; i < TALENT_SLOTS; ++i)
	{
		if (!m_TalentIcons[i])
			continue;

		if (i < static_cast<int>(talents.size()))
		{
			// 有天賦，顯示天賦圖標
			const auto &talent = talents[i];
			const std::string iconPath = RESOURCE_DIR + talent.GetIconPath();
			m_TalentIcons[i]->SetDrawable(ImagePoolManager::GetInstance().GetImage(iconPath));
		}
		else
		{
			// 空槽位，顯示預設圖標
			m_TalentIcons[i]->SetDrawable(
				ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_talentIcon/ui_buff_00.png"));
		}
	}
}

void PausePanel::DrawDebugUI()
{
	ImGui::Begin("Pause Panel Debug");

	// === Background Settings ===
	if (ImGui::CollapsingHeader("Background Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 size = glm::vec2(400.0f, 300.0f);
		static glm::vec2 pos = glm::vec2(0.0f, 0.0f);
		static bool posChanged = false, sizeChanged = false;

		if (sizeChanged)
		{
			m_PanelBackground->m_Transform.scale = size / m_PanelBackground->GetImageSize();
			sizeChanged = false;
		}
		if (posChanged)
		{
			m_PanelBackground->m_Transform.translation = pos;
			posChanged = false;
		}

		ImGui::InputFloat("Background Width", &size.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Background Height", &size.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Background X", &pos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Background Y", &pos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;

		if (!sizeChanged)
			size = m_PanelBackground->m_Transform.scale * m_PanelBackground->GetImageSize();
		if (!posChanged)
			pos = m_PanelBackground->m_Transform.translation;

		ImGui::LabelText("Current Scale X", "%.3f", m_PanelBackground->m_Transform.scale.x);
		ImGui::LabelText("Current Scale Y", "%.3f", m_PanelBackground->m_Transform.scale.y);
		ImGui::LabelText("Image Size X", "%.3f", m_PanelBackground->GetImageSize().x);
		ImGui::LabelText("Image Size Y", "%.3f", m_PanelBackground->GetImageSize().y);
		ImGui::LabelText("Scaled Size X", "%.3f", m_PanelBackground->GetScaledSize().x);
		ImGui::LabelText("Scaled Size Y", "%.3f", m_PanelBackground->GetScaledSize().y);
	}

	// === Talent Icons Settings ===
	if (ImGui::CollapsingHeader("Talent Icons Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static float iconY = 0.0f; // 設定初始值為 0.0f
		static float iconSpacing = 82.0f;
		static float iconScale = 2.0f; // 添加縮放控制
		static bool iconPosChanged = false;
		static bool iconScaleChanged = false;

		if (iconPosChanged || iconScaleChanged)
		{
			// 重新計算所有天賦圖標位置和縮放
			const float totalWidth = (TALENT_SLOTS - 1) * iconSpacing;
			const float startX = -totalWidth / 2.0f;

			for (int i = 0; i < TALENT_SLOTS && i < static_cast<int>(m_TalentIcons.size()); ++i)
			{
				if (m_TalentIcons[i])
				{
					m_TalentIcons[i]->m_Transform.translation = glm::vec2(startX + i * iconSpacing, iconY);
					m_TalentIcons[i]->m_Transform.scale = glm::vec2(iconScale, iconScale);
				}
			}
			iconPosChanged = false;
			iconScaleChanged = false;
		}

		ImGui::InputFloat("Icon Y Position", &iconY, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			iconPosChanged = true;
		ImGui::InputFloat("Icon Spacing", &iconSpacing, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			iconPosChanged = true;
		ImGui::InputFloat("Icon Scale", &iconScale, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			iconScaleChanged = true;

		// 顯示天賦信息
		const auto talentComp = m_PlayerTalentComponent.lock();
		if (talentComp)
		{
			const auto &talents = talentComp->GetTalents();
			ImGui::Text("Total Talents: %zu", talents.size());

			for (size_t i = 0; i < talents.size() && i < TALENT_SLOTS; ++i)
			{
				const auto &talent = talents[i];
				ImGui::Text("Slot %zu: %s (ID: %d)", i, talent.GetName().c_str(), talent.GetId());
			}
		}
		else
		{
			ImGui::Text("No TalentComponent available");
		}
	}

	// === Button Settings ===
	if (ImGui::CollapsingHeader("Button Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Resume Button
		static glm::vec2 resumeButtonPos = glm::vec2(0.0f, -100.0f);
		static bool resumeButtonPosChanged = false;

		if (resumeButtonPosChanged)
		{
			m_ResumeButton->m_Transform.translation = resumeButtonPos;
			resumeButtonPosChanged = false;
		}

		ImGui::InputFloat("Resume Button X", &resumeButtonPos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			resumeButtonPosChanged = true;
		ImGui::InputFloat("Resume Button Y", &resumeButtonPos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			resumeButtonPosChanged = true;

		if (!resumeButtonPosChanged)
			resumeButtonPos = m_ResumeButton->m_Transform.translation;

		// Menu Button
		static glm::vec2 menuButtonPos = glm::vec2(-110.0f, -100.0f);
		static bool menuButtonPosChanged = false;

		if (menuButtonPosChanged)
		{
			m_MenuButton->m_Transform.translation = menuButtonPos;
			menuButtonPosChanged = false;
		}

		ImGui::InputFloat("Menu Button X", &menuButtonPos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			menuButtonPosChanged = true;
		ImGui::InputFloat("Menu Button Y", &menuButtonPos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			menuButtonPosChanged = true;

		if (!menuButtonPosChanged)
			menuButtonPos = m_MenuButton->m_Transform.translation;

		// Setting Button
		static glm::vec2 settingButtonPos = glm::vec2(110.0f, -100.0f);
		static bool settingButtonPosChanged = false;

		if (settingButtonPosChanged)
		{
			m_SettingButton->m_Transform.translation = settingButtonPos;
			settingButtonPosChanged = false;
		}

		ImGui::InputFloat("Setting Button X", &settingButtonPos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			settingButtonPosChanged = true;
		ImGui::InputFloat("Setting Button Y", &settingButtonPos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			settingButtonPosChanged = true;

		if (!settingButtonPosChanged)
			settingButtonPos = m_SettingButton->m_Transform.translation;
	}

	// === Panel Control ===
	if (ImGui::CollapsingHeader("Panel Control", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Show Panel"))
		{
			Show();
		}
		ImGui::SameLine();
		if (ImGui::Button("Hide Panel"))
		{
			Hide();
		}

		ImGui::Text("Panel Visible: %s", IsVisible() ? "Yes" : "No");
	}

	ImGui::End();
}

void PausePanel::Show()
{
	if (m_IsAnimating && m_IsShowingAnimation)
		return; // 避免重複調用

	UIPanel::Show(); // 先顯示面板
	StartShowAnimation();
}

void PausePanel::Hide()
{
	if (m_IsAnimating && !m_IsShowingAnimation)
		return; // 避免重複調用

	StartHideAnimation();
	// 注意：不立即調用 UIPanel::Hide()，等動畫完成後再調用
}

void PausePanel::StartShowAnimation()
{
	m_IsAnimating = true;
	m_IsShowingAnimation = true;
	m_AnimationTimer = 0.0f;

	// 確保面板從隱藏位置開始
	UpdateAllElementsPosition(m_HiddenPosition);
}

void PausePanel::StartHideAnimation()
{
	m_IsAnimating = true;
	m_IsShowingAnimation = false;
	m_AnimationTimer = 0.0f;

	// 確保面板從可見位置開始
	UpdateAllElementsPosition(m_VisiblePosition);
}

void PausePanel::UpdateAnimation()
{
	if (!m_IsAnimating)
		return;

	m_AnimationTimer += Util::Time::GetDeltaTimeMs() / 1000.0f;
	const float progress = std::min(m_AnimationTimer / m_AnimationDuration, 1.0f);

	UpdatePanelPosition(progress);

	// 動畫完成
	if (progress >= 1.0f)
	{
		m_IsAnimating = false;

		if (!m_IsShowingAnimation)
		{
			// Hide 動畫完成，現在真正隱藏面板
			UIPanel::Hide();
		}
	}
}

void PausePanel::UpdatePanelPosition(float progress)
{
	const float easedProgress = EaseOutQuad(progress);

	glm::vec2 currentPos;
	if (m_IsShowingAnimation)
	{
		// Show: 從隱藏位置到可見位置
		currentPos = m_HiddenPosition + (m_VisiblePosition - m_HiddenPosition) * easedProgress;
	}
	else
	{
		// Hide: 從可見位置到隱藏位置
		currentPos = m_VisiblePosition + (m_HiddenPosition - m_VisiblePosition) * easedProgress;
	}

	// 統一更新所有元件位置
	UpdateAllElementsPosition(currentPos);
}

void PausePanel::UpdateAllElementsPosition(const glm::vec2 &panelPosition)
{
	// 更新面板背景位置
	m_PanelBackground->m_Transform.translation = panelPosition;

	// 更新所有按鈕位置（相對於面板背景位置）
	m_ResumeButton->m_Transform.translation = panelPosition + m_ResumeButtonOffset;
	m_MenuButton->m_Transform.translation = panelPosition + m_MenuButtonOffset;
	m_SettingButton->m_Transform.translation = panelPosition + m_SettingButtonOffset;

	// 更新天賦圖標位置
	const float iconSpacing = 82.0f; // 圖標間距（包含圖標大小）
	const float totalWidth = (TALENT_SLOTS - 1) * iconSpacing;
	const float startX = -totalWidth / 2.0f;
	const glm::vec2 talentIconsPosition = panelPosition + m_TalentIconsBaseOffset;

	for (int i = 0; i < TALENT_SLOTS && i < static_cast<int>(m_TalentIcons.size()); ++i)
	{
		if (m_TalentIcons[i])
		{
			m_TalentIcons[i]->m_Transform.translation = talentIconsPosition + glm::vec2(startX + i * iconSpacing, 0.0f);
		}
	}
}

float PausePanel::EaseOutQuad(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }
