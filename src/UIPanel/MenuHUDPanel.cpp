//
// Created by QuzzS on 2025/6/5.
//

#include "UIPanel/MenuHUDPanel.hpp"

#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Override/nGameObject.hpp"
#include "SaveManager.hpp"
#include "Scene/SceneManager.hpp"
#include "Tool/Tool.hpp"
#include "UIPanel/UIButton.hpp"
#include "Util/Color.hpp"
#include "Util/Input.hpp"
#include "Util/Time.hpp"
#include "Util/Timer.hpp"
#include "config.hpp"


void MenuHUDPanel::Start()
{
	// 設置面板名稱
	SetPanelName("menuHUD");

	// 檢查存檔狀態來決定按鈕策略
	auto &saveManager = SaveManager::GetInstance();

	if (saveManager.HasSaveData())
	{
		auto saveData = saveManager.GetSaveData();
		m_HasGameProgress = (saveData && saveData->isInGameProgress);
	}

	// 根據遊戲進度狀態調整按鈕位置和數量
	if (m_HasGameProgress)
	{
		// 有遊戲進度：顯示兩個按鈕（新遊戲 + 繼續遊戲）
		// 使用預設的左右位置
	}
	else
	{
		// 沒有遊戲進度：只顯示新遊戲按鈕，位置在中央
		m_NewGameButtonVisiblePos = glm::vec2(0.0f, -280.0f);
	}

	// 計算隱藏位置（屏幕下方外側）
	const float windowHeight = static_cast<float>(PTSD_Config::WINDOW_HEIGHT);
	const float offscreenOffset = 200.0f; // 額外的偏移量確保完全隱藏
	m_NewGameButtonHiddenPos = glm::vec2(m_NewGameButtonVisiblePos.x, -(windowHeight * 0.5f + offscreenOffset));
	m_ContinueGameButtonHiddenPos =
		glm::vec2(m_ContinueGameButtonVisiblePos.x, -(windowHeight * 0.5f + offscreenOffset));

	// 初始化按鈕
	InitNewGameButton();

	// 只有在有遊戲進度時才初始化繼續遊戲按鈕
	if (m_HasGameProgress)
	{
		InitContinueGameButton();
	}

	// 添加到場景渲染器
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		const auto renderer = scene->GetRoot().lock();
		for (const auto &gameObject : m_GameObjects)
		{
			renderer->AddChild(gameObject);
		}
	}

	// 設定初始位置為隱藏狀態
	UpdateButtonsPosition(0.0f);

	// 默認隱藏
	UIPanel::Hide();
}

void MenuHUDPanel::Update()
{
	// 更新動畫
	UpdateAnimation();

	// 更新按鈕
	if (m_NewGameButton)
		m_NewGameButton->Update();
	if (m_ContinueGameButton)
		m_ContinueGameButton->Update();
}

void MenuHUDPanel::Show()
{
	if (m_IsAnimating && m_IsShowingAnimation)
		return; // 避免重複調用

	UIPanel::Show(); // 先顯示面板
	StartShowAnimation();
}

void MenuHUDPanel::Hide()
{
	if (m_IsAnimating && !m_IsShowingAnimation)
		return; // 避免重複調用

	StartHideAnimation();
	// 注意：不立即調用 UIPanel::Hide()，等動畫完成後再調用
}

void MenuHUDPanel::InitNewGameButton()
{
	auto &img = ImagePoolManager::GetInstance();

	// 創建新遊戲按鈕回調函數
	auto onClick = []()
	{
		LOG_DEBUG("New Game button clicked - resetting game progress");
		AudioManager::GetInstance().PlaySFX("click");

		// 只重置關卡進度，保留遊戲幣等永久數據
		auto &sceneManager = SceneManager::GetInstance();
		sceneManager.ResetGameProgress();

		sceneManager.SetNextScene(Scene::SceneType::Lobby);
	};

	m_NewGameButton = std::make_shared<UIButton>(onClick, false);
	m_NewGameButton->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/button_continue.png"));
	m_NewGameButton->SetZIndex(3.0f);
	m_GameObjects.push_back(std::static_pointer_cast<nGameObject>(m_NewGameButton));

	// 創建新遊戲按鈕文字
	m_NewGameButtonText = std::make_shared<nGameObject>();
	m_NewGameButtonText->SetDrawable(
		img.GetText(RESOURCE_DIR "/Font/zpix.ttf", 32, "新遊戲", Util::Color(255, 255, 255), false));
	m_NewGameButtonText->SetZIndex(4.0f); // 比按鈕高一層
	m_GameObjects.push_back(m_NewGameButtonText);
}

void MenuHUDPanel::InitContinueGameButton()
{
	auto &img = ImagePoolManager::GetInstance();

	// 創建繼續遊戲按鈕回調函數
	auto onClick = []()
	{
		LOG_DEBUG("Continue Game button clicked");
		AudioManager::GetInstance().PlaySFX("click");

		// 檢查是否有存檔數據
		auto &saveManager = SaveManager::GetInstance();
		if (saveManager.HasSaveData())
		{
			// 使用正常的場景切換機制
			auto &sceneManager = SceneManager::GetInstance();
			// 根據存檔狀態決定目標場景
			auto saveData = saveManager.GetSaveData();
			if (saveData && saveData->isInGameProgress)
			{
				sceneManager.SetNextScene(Scene::SceneType::Dungeon);
			}
			else
			{
				sceneManager.SetNextScene(Scene::SceneType::Lobby);
			}
		}
		else
		{
			LOG_INFO("No save data found for continue game");
		}
	};

	m_ContinueGameButton = std::make_shared<UIButton>(onClick, false);
	m_ContinueGameButton->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/button_continue.png"));
	m_ContinueGameButton->SetZIndex(3.0f);
	m_GameObjects.push_back(std::static_pointer_cast<nGameObject>(m_ContinueGameButton));

	// 創建繼續遊戲按鈕文字
	m_ContinueGameButtonText = std::make_shared<nGameObject>();
	m_ContinueGameButtonText->SetDrawable(
		img.GetText(RESOURCE_DIR "/Font/zpix.ttf", 32, "繼續遊戲", Util::Color(255, 255, 255), false));
	m_ContinueGameButtonText->SetZIndex(4.0f); // 比按鈕高一層
	m_GameObjects.push_back(m_ContinueGameButtonText);
}

void MenuHUDPanel::StartShowAnimation()
{
	m_IsAnimating = true;
	m_IsShowingAnimation = true;
	m_AnimationTimer.Restart();

	// 確保按鈕從隱藏位置開始
	UpdateButtonsPosition(0.0f);
}

void MenuHUDPanel::StartHideAnimation()
{
	m_IsAnimating = true;
	m_IsShowingAnimation = false;
	m_AnimationTimer.Restart();

	// 確保按鈕從可見位置開始
	UpdateButtonsPosition(1.0f);
}

void MenuHUDPanel::UpdateAnimation()
{
	if (!m_IsAnimating)
		return;

	m_AnimationTimer.Update();
	const float progress = m_AnimationTimer.GetEasedProgress(Util::Timer::EaseOutQuad);

	UpdateButtonsPosition(progress);

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

void MenuHUDPanel::UpdateButtonsPosition(float progress)
{
	glm::vec2 newGameCurrentPos;
	glm::vec2 continueGameCurrentPos;

	if (m_IsShowingAnimation)
	{
		// Show: 從隱藏位置到可見位置
		newGameCurrentPos =
			m_NewGameButtonHiddenPos + (m_NewGameButtonVisiblePos - m_NewGameButtonHiddenPos) * progress;

		// 只有在繼續遊戲按鈕存在時才計算其位置
		if (m_ContinueGameButton)
		{
			continueGameCurrentPos = m_ContinueGameButtonHiddenPos +
				(m_ContinueGameButtonVisiblePos - m_ContinueGameButtonHiddenPos) * progress;
		}
	}
	else
	{
		// Hide: 從可見位置到隱藏位置
		newGameCurrentPos =
			m_NewGameButtonVisiblePos + (m_NewGameButtonHiddenPos - m_NewGameButtonVisiblePos) * progress;

		// 只有在繼續遊戲按鈕存在時才計算其位置
		if (m_ContinueGameButton)
		{
			continueGameCurrentPos = m_ContinueGameButtonVisiblePos +
				(m_ContinueGameButtonHiddenPos - m_ContinueGameButtonVisiblePos) * progress;
		}
	}

	// 更新按鈕位置
	if (m_NewGameButton)
	{
		m_NewGameButton->m_Transform.translation = newGameCurrentPos;
	}
	if (m_ContinueGameButton)
	{
		m_ContinueGameButton->m_Transform.translation = continueGameCurrentPos;
	}

	// 更新文字位置（相對於按鈕位置）
	if (m_NewGameButtonText)
	{
		m_NewGameButtonText->m_Transform.translation = newGameCurrentPos + m_TextOffset;
	}
	if (m_ContinueGameButtonText)
	{
		m_ContinueGameButtonText->m_Transform.translation = continueGameCurrentPos + m_TextOffset;
	}
}

bool MenuHUDPanel::IsMouseClickingOnButtons() const
{
	// 如果面板不可見或正在隱藏動畫，則不檢查碰撞
	if (!IsVisible() || (m_IsAnimating && !m_IsShowingAnimation))
		return false;

	// 獲取滑鼠座標
	glm::vec2 mousePos = Tool::GetMouseCoord();

	// 檢查是否點擊到新遊戲按鈕
	if (m_NewGameButton)
	{
		glm::vec2 buttonPos = m_NewGameButton->m_Transform.translation;
		glm::vec2 buttonSize = m_NewGameButton->GetScaledSize();

		if (mousePos.x >= buttonPos.x - buttonSize.x / 2.0f && mousePos.x <= buttonPos.x + buttonSize.x / 2.0f &&
			mousePos.y >= buttonPos.y - buttonSize.y / 2.0f && mousePos.y <= buttonPos.y + buttonSize.y / 2.0f)
		{
			return true;
		}
	}

	// 檢查是否點擊到繼續遊戲按鈕（只有在按鈕存在時才檢查）
	if (m_ContinueGameButton)
	{
		glm::vec2 buttonPos = m_ContinueGameButton->m_Transform.translation;
		glm::vec2 buttonSize = m_ContinueGameButton->GetScaledSize();

		if (mousePos.x >= buttonPos.x - buttonSize.x / 2.0f && mousePos.x <= buttonPos.x + buttonSize.x / 2.0f &&
			mousePos.y >= buttonPos.y - buttonSize.y / 2.0f && mousePos.y <= buttonPos.y + buttonSize.y / 2.0f)
		{
			return true;
		}
	}

	return false; // 沒有點擊到任何按鈕
}
