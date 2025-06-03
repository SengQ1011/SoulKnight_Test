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


void PausePanel::Start()
{
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
	m_PanelBackground->m_Transform.translation = glm::vec2(0.0f, 0.0f);
	m_GameObjects.push_back(m_PanelBackground);

	// 創建 ResumeButton - 關閉PausePanel返回游戲
	std::function<void()> resume_function = [this]() { this->Hide(); };
	m_ResumeButton = std::make_shared<UIButton>(resume_function, false);
	m_ResumeButton->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_pausePanel/button_resume.png"));
	m_ResumeButton->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_ResumeButton->m_Transform.translation = glm::vec2(0.0f, -100.0f);
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
	// 緊貼ResumeButton的左邊位置（假設按鈕寬度約100，間距10）
	m_MenuButton->m_Transform.translation = glm::vec2(-224.0f, -100.0f);
	m_GameObjects.push_back(m_MenuButton);

	// 創建 SettingButton - 呼叫SettingPanel
	std::function<void()> setting_function = []()
	{
		// 顯示設定面板
		UIManager::GetInstance().ShowPanel("setting");
	};
	m_SettingButton = std::make_shared<UIButton>(setting_function, false);
	m_SettingButton->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_pausePanel/button_setting.png"));
	m_SettingButton->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	// 緊貼ResumeButton的右邊位置
	m_SettingButton->m_Transform.translation = glm::vec2(224.0f, -100.0f);
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

	// 默認隱藏面板
	Hide();
}

void PausePanel::Update()
{
	// 更新天賦顯示
	UpdateTalentIcons();

	DrawDebugUI();
	for (const std::shared_ptr<nGameObject> &gameObject : m_GameObjects)
	{
		gameObject->Update();
	}

	// 更新天賦圖標
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

	// 計算起始位置（中心對齊）
	const float iconSpacing = 82.0f; // 圖標間距（包含圖標大小）
	const float totalWidth = (TALENT_SLOTS - 1) * iconSpacing;
	const float startX = -totalWidth / 2.0f;
	const float iconY = 0.0f; // 天賦圖標的Y位置（正中央）

	for (int i = 0; i < TALENT_SLOTS; ++i)
	{
		auto talentIcon = std::make_shared<nGameObject>();

		// 設定預設圖標（空槽位）
		talentIcon->SetDrawable(
			ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_talentIcon/ui_buff_00.png"));

		talentIcon->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
		talentIcon->m_Transform.translation = glm::vec2(startX + i * iconSpacing, iconY);

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
