//
// Created by QuzzS on 2025/5/1.
//

#include "UIPanel/SettingPanel.hpp"

#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Override/nGameObject.hpp"
#include "Scene/SceneManager.hpp"
#include "UIPanel/UIButton.hpp"
#include "UIPanel/UIManager.hpp"
#include "UIPanel/UISlider.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "config.hpp"


void SettingPanel::Start()
{
	// 設置面板名稱用於輸入阻擋檢查
	SetPanelName("setting");

	// 創建遮罩層
	m_OverLay = std::make_shared<nGameObject>();
	m_OverLay->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/overlay_black.png"));
	m_OverLay->SetZIndex(91.0f);
	m_GameObjects.push_back(m_OverLay);

	// 創建面板背景
	m_PanelBackground = std::make_shared<nGameObject>();
	m_PanelBackground->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/background_menuPanel.png"));
	m_PanelBackground->SetZIndex(m_OverLay->GetZIndex() + 0.1f);
	m_GameObjects.push_back(m_PanelBackground);

	// 計算隱藏位置（螢幕下方外側）
	const float windowHeight = static_cast<float>(PTSD_Config::WINDOW_HEIGHT);
	const float panelHeight = m_PanelBackground->GetScaledSize().y;
	m_HiddenPosition = glm::vec2(0.0f, -(windowHeight * 0.5f + panelHeight));
	m_VisiblePosition = m_BackgroundOffset;

	// 主音量的監聽和控制function
	const auto controlMasterVoice = [](const float newValue) { AudioManager::GetInstance().SetMasterVolume(newValue); };
	const auto listenMasterVoice = []() -> float { return AudioManager::GetInstance().GetMasterVolume(); };

	// 主音量的軌道
	auto track1 = std::make_shared<nGameObject>();
	track1->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/ui_progress_blue.png"));
	m_GameObjects.push_back(track1);

	// 主音量滑桿
	m_SliderMasterVolume =
		std::make_shared<UISlider>(listenMasterVoice, track1, glm::vec2(7.0f, 4.0f), true,
								   RESOURCE_DIR "/UI/ui_settingPanel/button_volume.png", controlMasterVoice);
	m_SliderMasterVolume->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/ui_progress_bg.png"));
	m_SliderMasterVolume->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_SliderMasterVolume->Start();
	const auto slider_button1 = m_SliderMasterVolume->GetButton();
	m_GameObjects.push_back(slider_button1);
	m_GameObjects.push_back(m_SliderMasterVolume);

	// BGM 音量
	const auto controlBGMVoice = [](const float newValue) { AudioManager::GetInstance().SetBGMVolume(newValue); };
	const auto listenBGMVoice = []() -> float { return AudioManager::GetInstance().GetBGMVolume(); };

	auto track2 = std::make_shared<nGameObject>();
	track2->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/ui_progress_blue.png"));
	m_GameObjects.push_back(track2);

	m_SliderBGMVolume =
		std::make_shared<UISlider>(listenBGMVoice, track2, glm::vec2(7.0f, 4.0f), true,
								   RESOURCE_DIR "/UI/ui_settingPanel/button_volume.png", controlBGMVoice);
	m_SliderBGMVolume->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/ui_progress_bg.png"));
	m_SliderBGMVolume->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_SliderBGMVolume->Start();
	const auto slider_button2 = m_SliderBGMVolume->GetButton();
	m_GameObjects.push_back(slider_button2);
	m_GameObjects.push_back(m_SliderBGMVolume);

	// SFX 音量
	const auto controlSFXVoice = [](const float newValue) { AudioManager::GetInstance().SetSFXVolume(newValue); };
	const auto listenSFXVoice = []() -> float { return AudioManager::GetInstance().GetSFXVolume(); };

	auto track3 = std::make_shared<nGameObject>();
	track3->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/ui_progress_blue.png"));
	m_GameObjects.push_back(track3);

	m_SliderSFXVolume =
		std::make_shared<UISlider>(listenSFXVoice, track3, glm::vec2(7.0f, 4.0f), true,
								   RESOURCE_DIR "/UI/ui_settingPanel/button_volume.png", controlSFXVoice);
	m_SliderSFXVolume->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/ui_progress_bg.png"));
	m_SliderSFXVolume->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_SliderSFXVolume->Start();
	const auto slider_button3 = m_SliderSFXVolume->GetButton();
	m_GameObjects.push_back(slider_button3);
	m_GameObjects.push_back(m_SliderSFXVolume);

	// 關閉按鈕
	std::function<void()> button_function = [this]() { this->Hide(); };
	m_CloseButton = std::make_shared<UIButton>(button_function, false);
	m_CloseButton->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/button_close.png"));
	m_CloseButton->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_GameObjects.push_back(m_CloseButton);

	// 初始化側邊按鈕
	InitializeSideButtons();

	// 添加到場景渲染器
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		const auto renderer = scene->GetRoot().lock();
		renderer->AddChild(m_OverLay);
		renderer->AddChild(m_PanelBackground);
		renderer->AddChild(m_CloseButton);
		renderer->AddChild(track1);
		renderer->AddChild(track2);
		renderer->AddChild(track3);
		renderer->AddChild(slider_button1);
		renderer->AddChild(slider_button2);
		renderer->AddChild(slider_button3);
		renderer->AddChild(m_SliderMasterVolume);
		renderer->AddChild(m_SliderBGMVolume);
		renderer->AddChild(m_SliderSFXVolume);
		renderer->AddChild(m_SideButton1);
		renderer->AddChild(m_SideButton2);
	}

	// 設定初始位置為隱藏狀態
	UpdateAllElementsPosition(m_HiddenPosition);

	UIPanel::Hide();
}

void SettingPanel::Update()
{
	// 優先更新動畫
	UpdateAnimation();

	// 更新所有元件（移除動畫期間的性能優化，確保滑塊元件位置同步）
	DrawDebugUI(); // 注解掉 Debug UI
	for (const std::shared_ptr<nGameObject> &gameObject : m_GameObjects)
	{
		gameObject->Update();
	}
}

void SettingPanel::Show()
{
	if (m_IsAnimating && m_IsShowingAnimation)
		return; // 避免重複調用

	UIPanel::Show(); // 先顯示面板
	StartShowAnimation();
}

void SettingPanel::Hide()
{
	if (m_IsAnimating && !m_IsShowingAnimation)
		return; // 避免重複調用

	StartHideAnimation();
	// 注意：不立即調用 UIPanel::Hide()，等動畫完成後再調用
}

void SettingPanel::StartShowAnimation()
{
	m_IsAnimating = true;
	m_IsShowingAnimation = true;
	m_AnimationTimer.Restart();

	// 確保面板從隱藏位置開始
	UpdateAllElementsPosition(m_HiddenPosition);
}

void SettingPanel::StartHideAnimation()
{
	m_IsAnimating = true;
	m_IsShowingAnimation = false;
	m_AnimationTimer.Restart();

	// 確保面板從可見位置開始
	UpdateAllElementsPosition(m_VisiblePosition);
}

void SettingPanel::UpdateAnimation()
{
	if (!m_IsAnimating)
		return;

	m_AnimationTimer.Update();
	const float progress = m_AnimationTimer.GetEasedProgress(Util::Timer::EaseOutQuad);

	UpdatePanelPosition(progress);

	// 動畫完成
	if (m_AnimationTimer.IsFinished())
	{
		m_IsAnimating = false;

		if (!m_IsShowingAnimation)
		{
			// Hide 動畫完成，現在真正隱藏面板
			UIPanel::Hide();
		}
	}
}

void SettingPanel::UpdatePanelPosition(float progress)
{
	glm::vec2 currentPos;
	if (m_IsShowingAnimation)
	{
		// Show: 從隱藏位置到可見位置
		currentPos = m_HiddenPosition + (m_VisiblePosition - m_HiddenPosition) * progress;
	}
	else
	{
		// Hide: 從可見位置到隱藏位置
		currentPos = m_VisiblePosition + (m_HiddenPosition - m_VisiblePosition) * progress;
	}

	// 統一更新所有元件位置
	UpdateAllElementsPosition(currentPos);
}

void SettingPanel::UpdateAllElementsPosition(const glm::vec2 &panelPosition)
{
	// 更新面板背景位置
	m_PanelBackground->m_Transform.translation = panelPosition;

	// 更新所有元件位置（相對於面板背景位置）
	m_SliderMasterVolume->m_Transform.translation = panelPosition + m_SliderOffset1;
	m_SliderBGMVolume->m_Transform.translation = panelPosition + m_SliderOffset2;
	m_SliderSFXVolume->m_Transform.translation = panelPosition + m_SliderOffset3;
	m_CloseButton->m_Transform.translation = panelPosition + m_CloseButtonOffset;

	// 更新側邊按鈕位置
	UpdateSideButtonsPosition(panelPosition);
}

void SettingPanel::InitializeSideButtons()
{
	// 創建側邊按鈕1 - UIButton (可點擊，切換到 KeyPanel)
	std::function<void()> switchToKeyPanel = []()
	{
		UIManager::GetInstance().HidePanel("setting");
		UIManager::GetInstance().ShowPanel("key");
	};
	m_SideButton1 = std::make_shared<UIButton>(switchToKeyPanel, false);
	m_SideButton1->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/key/sideButton.png"));
	m_SideButton1->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_GameObjects.push_back(m_SideButton1);

	// 創建側邊按鈕2 - nGameObject (純顯示)
	m_SideButton2 = std::make_shared<nGameObject>();
	m_SideButton2->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/key/sideButton.png"));
	m_SideButton2->SetZIndex(m_PanelBackground->GetZIndex() - 0.05f);
	m_GameObjects.push_back(m_SideButton2);
}

void SettingPanel::UpdateSideButtonsPosition(const glm::vec2 &panelPosition)
{
	// 計算第一個按鈕位置
	glm::vec2 offset;
	offset.x = (m_PanelBackground->GetScaledSize().x + m_SideButton1->GetScaledSize().x) / -2.0f + 7.0f;
	offset.y = m_PanelBackground->GetScaledSize().y / 2.0f - m_SideButton1->GetScaledSize().y - 32.0f;
	m_SideButton1->m_Transform.translation = panelPosition + offset;

	// 計算第二個按鈕位置
	float offsetY = (m_SideButton1->GetScaledSize().y + m_SideButton2->GetScaledSize().y) / 2.0f + 32.0f;
	m_SideButton2->m_Transform.translation = m_SideButton1->m_Transform.translation - glm::vec2(0.0f, offsetY);
}

void SettingPanel::DrawDebugUI()
{
	ImGui::Begin("UI Settings");

	// === Background Settings ===
	if (ImGui::CollapsingHeader("Background Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 size = glm::vec2(760.0f, 528.0f);
		static glm::vec2 pos = glm::vec2(0, 0);
		static bool posChanged = false, sizeChanged = false;

		if (sizeChanged)
		{
			m_PanelBackground->m_Transform.scale = size / m_PanelBackground->GetImageSize();
			sizeChanged = false;
		}
		if (posChanged)
		{
			// 使用統一位置更新方法
			UpdateAllElementsPosition(pos);
			posChanged = false;
		}

		ImGui::InputFloat("Setting Width##bg", &size.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Setting Height##bg", &size.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Setting x##bg", &pos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Setting y##bg", &pos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;

		if (!sizeChanged)
			size = m_PanelBackground->m_Transform.scale * m_PanelBackground->GetImageSize();
		if (!posChanged)
			pos = m_PanelBackground->m_Transform.translation;

		ImGui::LabelText("scale.x", "%.3f", m_PanelBackground->m_Transform.scale.x);
		ImGui::LabelText("scale.y", "%.3f", m_PanelBackground->m_Transform.scale.y);
	}

	// === Side Buttons Settings ===
	if (ImGui::CollapsingHeader("Side Buttons Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Side Button 1 (UIButton - Switch to Key):");
		ImGui::LabelText("Position X##side1", "%.3f", m_SideButton1->m_Transform.translation.x);
		ImGui::LabelText("Position Y##side1", "%.3f", m_SideButton1->m_Transform.translation.y);

		ImGui::Text("Side Button 2 (nGameObject - Display):");
		ImGui::LabelText("Position X##side2", "%.3f", m_SideButton2->m_Transform.translation.x);
		ImGui::LabelText("Position Y##side2", "%.3f", m_SideButton2->m_Transform.translation.y);

		if (ImGui::Button("Test Switch to Key Panel"))
		{
			UIManager::GetInstance().HidePanel("setting");
			UIManager::GetInstance().ShowPanel("key");
		}
	}

	// === Animation Debug ===
	if (ImGui::CollapsingHeader("Animation Debug", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Is Animating: %s", m_IsAnimating ? "Yes" : "No");
		ImGui::Text("Is Showing: %s", m_IsShowingAnimation ? "Yes" : "No");
		ImGui::Text("Animation Timer: %.3f", m_AnimationTimer.GetProgress());
		ImGui::Text("Visible Pos: (%.1f, %.1f)", m_VisiblePosition.x, m_VisiblePosition.y);
		ImGui::Text("Hidden Pos: (%.1f, %.1f)", m_HiddenPosition.x, m_HiddenPosition.y);
		ImGui::Text("Current Pos: (%.1f, %.1f)", m_PanelBackground->m_Transform.translation.x,
					m_PanelBackground->m_Transform.translation.y);

		// 顯示各元件位置
		ImGui::Text("Slider1 Pos: (%.1f, %.1f)", m_SliderMasterVolume->m_Transform.translation.x,
					m_SliderMasterVolume->m_Transform.translation.y);
		ImGui::Text("CloseButton Pos: (%.1f, %.1f)", m_CloseButton->m_Transform.translation.x,
					m_CloseButton->m_Transform.translation.y);

		if (ImGui::Button("Test Show"))
		{
			Show();
		}
		ImGui::SameLine();
		if (ImGui::Button("Test Hide"))
		{
			Hide();
		}
	}

	// === Offset Settings ===
	if (ImGui::CollapsingHeader("Offset Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static bool offsetChanged = false;

		ImGui::InputFloat2("Background Offset", &m_BackgroundOffset.x);
		if (ImGui::IsItemDeactivatedAfterEdit())
			offsetChanged = true;

		ImGui::InputFloat2("Slider1 Offset", &m_SliderOffset1.x);
		if (ImGui::IsItemDeactivatedAfterEdit())
			offsetChanged = true;

		ImGui::InputFloat2("Slider2 Offset", &m_SliderOffset2.x);
		if (ImGui::IsItemDeactivatedAfterEdit())
			offsetChanged = true;

		ImGui::InputFloat2("Slider3 Offset", &m_SliderOffset3.x);
		if (ImGui::IsItemDeactivatedAfterEdit())
			offsetChanged = true;

		ImGui::InputFloat2("CloseButton Offset", &m_CloseButtonOffset.x);
		if (ImGui::IsItemDeactivatedAfterEdit())
			offsetChanged = true;

		if (offsetChanged)
		{
			// 即時更新位置
			if (!m_IsAnimating)
			{
				UpdateAllElementsPosition(m_PanelBackground->m_Transform.translation);
			}
			offsetChanged = false;
		}
	}

	ImGui::End();
}
