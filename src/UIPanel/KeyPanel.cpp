//
// Created by QuzzS on 2025/5/1.
//

#include "UIPanel/KeyPanel.hpp"

#include "ImagePoolManager.hpp"
#include "Override/nGameObject.hpp"
#include "Scene/SceneManager.hpp"
#include "UIPanel/UIButton.hpp"
#include "UIPanel/UIManager.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "config.hpp"

void KeyPanel::Start()
{
	// 設置面板名稱用於輸入阻擋檢查
	SetPanelName("key");

	// 創建遮罩層
	m_OverLay = std::make_shared<nGameObject>();
	m_OverLay->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/overlay_black.png"));
	m_OverLay->SetZIndex(91.0f);
	m_GameObjects.push_back(m_OverLay);

	// 創建面板背景
	m_PanelBackground = std::make_shared<nGameObject>();
	m_PanelBackground->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/key/background_keyPanel.png"));
	m_PanelBackground->SetZIndex(m_OverLay->GetZIndex() + 0.1f);
	m_GameObjects.push_back(m_PanelBackground);

	// 計算隱藏位置（螢幕下方外側）
	const float windowHeight = static_cast<float>(PTSD_Config::WINDOW_HEIGHT);
	const float panelHeight = m_PanelBackground->GetScaledSize().y;
	m_HiddenPosition = glm::vec2(0.0f, -(windowHeight * 0.5f + panelHeight));
	m_VisiblePosition = m_BackgroundOffset;

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
		renderer->AddChild(m_SideButton1);
		renderer->AddChild(m_SideButton2);
	}

	// 設定初始位置為隱藏狀態
	UpdateAllElementsPosition(m_HiddenPosition);

	UIPanel::Hide();
}

void KeyPanel::Update()
{
	// 優先更新動畫
	UpdateAnimation();

	// 更新所有元件
	for (const std::shared_ptr<nGameObject> &gameObject : m_GameObjects)
	{
		gameObject->Update();
	}
}

void KeyPanel::Show()
{
	if (m_IsAnimating && m_IsShowingAnimation)
		return; // 避免重複調用

	UIPanel::Show(); // 先顯示面板
	StartShowAnimation();
}

void KeyPanel::Hide()
{
	if (m_IsAnimating && !m_IsShowingAnimation)
		return; // 避免重複調用

	StartHideAnimation();
	// 注意：不立即調用 UIPanel::Hide()，等動畫完成後再調用
}

void KeyPanel::StartShowAnimation()
{
	m_IsAnimating = true;
	m_IsShowingAnimation = true;
	m_AnimationTimer.Restart();

	// 確保面板從隱藏位置開始
	UpdateAllElementsPosition(m_HiddenPosition);
}

void KeyPanel::StartHideAnimation()
{
	m_IsAnimating = true;
	m_IsShowingAnimation = false;
	m_AnimationTimer.Restart();

	// 確保面板從可見位置開始
	UpdateAllElementsPosition(m_VisiblePosition);
}

void KeyPanel::UpdateAnimation()
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

void KeyPanel::UpdatePanelPosition(float progress)
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

void KeyPanel::UpdateAllElementsPosition(const glm::vec2 &panelPosition)
{
	// 更新面板背景位置
	m_PanelBackground->m_Transform.translation = panelPosition;

	// 更新關閉按鈕位置
	m_CloseButton->m_Transform.translation = panelPosition + m_CloseButtonOffset;

	// 更新側邊按鈕位置
	UpdateSideButtonsPosition(panelPosition);
}

void KeyPanel::InitializeSideButtons()
{
	// 創建側邊按鈕1 - nGameObject (純顯示)
	m_SideButton1 = std::make_shared<nGameObject>();
	m_SideButton1->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/key/sideButton.png"));
	m_SideButton1->SetZIndex(m_PanelBackground->GetZIndex() - 0.05f);
	m_GameObjects.push_back(m_SideButton1);

	// 創建側邊按鈕2 - UIButton (可點擊，切換到 SettingPanel)
	std::function<void()> switchToSettingPanel = []()
	{
		UIManager::GetInstance().HidePanel("key");
		UIManager::GetInstance().ShowPanel("setting");
	};
	m_SideButton2 = std::make_shared<UIButton>(switchToSettingPanel, false);
	m_SideButton2->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/key/sideButton.png"));
	m_SideButton2->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_GameObjects.push_back(m_SideButton2);
}

void KeyPanel::UpdateSideButtonsPosition(const glm::vec2 &panelPosition)
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

void KeyPanel::DrawDebugUI()
{
	ImGui::Begin("Key Panel Debug");

	// === Background Settings ===
	if (ImGui::CollapsingHeader("Background Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 size = m_PanelBackground->m_Transform.scale * m_PanelBackground->GetImageSize();
		static glm::vec2 pos = m_PanelBackground->m_Transform.translation;
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

		ImGui::InputFloat("Key Width##bg", &size.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Key Height##bg", &size.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Key x##bg", &pos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Key y##bg", &pos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;

		if (!sizeChanged)
			size = m_PanelBackground->m_Transform.scale * m_PanelBackground->GetImageSize();
		if (!posChanged)
			pos = m_PanelBackground->m_Transform.translation;

		ImGui::LabelText("scale.x", "%.3f", m_PanelBackground->m_Transform.scale.x);
		ImGui::LabelText("scale.y", "%.3f", m_PanelBackground->m_Transform.scale.y);
	}

	// === Close Button Settings ===
	if (ImGui::CollapsingHeader("Close Button Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 closeButtonPos = m_CloseButtonOffset;
		static bool closeButtonPosChanged = false;

		if (closeButtonPosChanged)
		{
			m_CloseButtonOffset = closeButtonPos;
			m_CloseButton->m_Transform.translation = m_PanelBackground->m_Transform.translation + m_CloseButtonOffset;
			closeButtonPosChanged = false;
		}

		ImGui::InputFloat("Close Button X##close", &closeButtonPos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			closeButtonPosChanged = true;
		ImGui::InputFloat("Close Button Y##close", &closeButtonPos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			closeButtonPosChanged = true;

		if (!closeButtonPosChanged)
			closeButtonPos = m_CloseButtonOffset;
	}

	// === Side Buttons Settings ===
	if (ImGui::CollapsingHeader("Side Buttons Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Side Button 1 (nGameObject):");
		ImGui::LabelText("Position X##side1", "%.3f", m_SideButton1->m_Transform.translation.x);
		ImGui::LabelText("Position Y##side1", "%.3f", m_SideButton1->m_Transform.translation.y);

		ImGui::Text("Side Button 2 (UIButton):");
		ImGui::LabelText("Position X##side2", "%.3f", m_SideButton2->m_Transform.translation.x);
		ImGui::LabelText("Position Y##side2", "%.3f", m_SideButton2->m_Transform.translation.y);
	}

	// === Animation Debug ===
	if (ImGui::CollapsingHeader("Animation Debug", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Is Animating: %s", m_IsAnimating ? "Yes" : "No");
		ImGui::Text("Is Showing Animation: %s", m_IsShowingAnimation ? "Yes" : "No");
		ImGui::Text("Animation Progress: %.3f", m_AnimationTimer.GetProgress());
		ImGui::Text("Visible Position: (%.1f, %.1f)", m_VisiblePosition.x, m_VisiblePosition.y);
		ImGui::Text("Hidden Position: (%.1f, %.1f)", m_HiddenPosition.x, m_HiddenPosition.y);
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

		if (ImGui::Button("Switch to Setting Panel"))
		{
			UIManager::GetInstance().HidePanel("key");
			UIManager::GetInstance().ShowPanel("setting");
		}
	}

	ImGui::End();
}
