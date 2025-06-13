//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/MainMenu_Scene.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "SaveManager.hpp"
#include "Scene/SceneManager.hpp"
#include "Tool/Tool.hpp"
#include "UIPanel/MenuHUDPanel.hpp"
#include "UIPanel/SettingPanel.hpp"
#include "UIPanel/UIButton.hpp"
#include "UIPanel/UIManager.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Text.hpp"
#include "Util/Time.hpp"
#include "config.hpp"

void MainMenuScene::Start()
{
	LOG_DEBUG("Entering Main Menu Scene");

	// 確保獲取場景數據（主菜單可能沒有數據，這是正常的）
	if (!m_SceneData)
	{
		Scene::Download();
	}

	// 主菜單不需要數據也能正常運行，所以不檢查失敗情況
	LOG_DEBUG("Scene data status: {}", m_SceneData ? "Available" : "Not available");

	// 初始化界面物件
	InitBackground();
	InitTitleAndDecor();
	InitTextLabels();
	InitUIManager();
	InitSettingButton();
	InitDeleteDataButton();
	InitMenuHUDPanel();
	InitAudioManager();
	InitSlideAnimation();

	m_Root->AddChild(m_Background);
	m_Root->AddChild(m_SettingButton);
	m_Root->AddChild(m_DeleteDataButton);
	m_Root->AddChild(m_RedShawl);
	m_Root->AddChild(m_Title);
	m_Root->AddChild(m_Version);
	m_Root->AddChild(m_Text);

	FlushPendingObjectsToRendererAndCamera();
}

void MainMenuScene::Update()
{
	m_Root->Update();
	m_SettingButton->Update();
	m_DeleteDataButton->Update();

	// 更新MenuHUDPanel
	if (m_MenuHUDPanel)
		m_MenuHUDPanel->Update();

	// AudioManager::GetInstance().DrawDebugUI(); //測試用的
	UIManager::GetInstance().Update();

	// 更新動畫效果
	UpdateAnimations();
	UpdateTextBlinkEffect();

	// 檢測左鍵點擊且沒有點擊到UI元素
	if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB) && !IsMouseClickingOnButtons())
	{
		if (!m_ShowMenuButtons)
		{
			// 第一次點擊：讓 m_Text 消失，顯示按鈕
			m_Text->SetVisible(false);
			m_ShowMenuButtons = true;

			if (m_MenuHUDPanel)
				m_MenuHUDPanel->Show();

			// 開始平移顯示 m_RedShawl
			if (!m_SlideTimer.IsRunning() && !m_RedShawl->IsVisible())
			{
				StartSlideAnimation();
			}
		}
		else
		{
			// 第二次點擊：讓 m_Text 重新顯示，隱藏按鈕
			m_Text->SetVisible(true);
			m_ShowMenuButtons = false;

			if (m_MenuHUDPanel)
				m_MenuHUDPanel->Hide();
		}
	}
}

void MainMenuScene::Exit()
{
	LOG_DEBUG("Main Menu exited");
	AudioManager::GetInstance().PauseBGM();
}

void MainMenuScene::Upload()
{
	// MainMenu 場景不應該上傳遊戲進度數據
	// 避免覆寫其他場景已經設定的數據
	LOG_DEBUG("MainMenu_Scene::Upload() - skipping upload to avoid data overwrite");
}

Scene::SceneType MainMenuScene::Change()
{
	// if (Util::Input::IsKeyDown(Util::Keycode::RETURN) || Util::Input::IsKeyDown(Util::Keycode::SPACE))
	// {
	// 	AudioManager::GetInstance().PlaySFX("click");
	// 	SceneManager::GetInstance().StartGame();
	// 	// return SceneType::Lobby;
	// }
	return Scene::SceneType::Null;
}

void MainMenuScene::InitBackground()
{
	m_Background = std::make_shared<nGameObject>();
	auto &img = ImagePoolManager::GetInstance();
	m_Background->SetDrawable(img.GetImage(RESOURCE_DIR "/MainMenu/MainMenuBackground.png"));
	m_Background->SetZIndex(0);
}

void MainMenuScene::InitTitleAndDecor()
{
	m_Title = std::make_shared<nGameObject>();
	auto &img = ImagePoolManager::GetInstance();
	m_Title->SetDrawable(img.GetImage(RESOURCE_DIR "/MainMenu/Title.png"));
	m_Title->SetZIndex(2);
	m_Title->m_Transform.translation = glm::vec2(-234.0f, 221.5f);

	m_RedShawl = std::make_shared<nGameObject>();
	m_RedShawl->SetDrawable(img.GetImage(RESOURCE_DIR "/MainMenu/RedShawl.png"));
	m_RedShawl->SetZIndex(1);
	m_RedShawl->SetVisible(false); // 初始隱藏紅披風
	m_RedShawl->m_Transform.translation =
		glm::vec2((static_cast<float>(PTSD_Config::WINDOW_WIDTH) + m_RedShawl->GetScaledSize().x) / 2, 22.0f);
}

void MainMenuScene::InitTextLabels()
{
	m_Text = std::make_shared<nGameObject>();
	auto &img = ImagePoolManager::GetInstance();
	m_Text->SetDrawable(
		img.GetText(RESOURCE_DIR "/Font/zpix.ttf", 20, "點擊呼叫司令部OwOb", Util::Color(255, 255, 255)));
	m_Text->SetZIndex(2);
	m_Text->m_Transform.translation = glm::vec2(11.0f, -300.0f);

	m_Version = std::make_shared<nGameObject>();
	m_Version->SetDrawable(img.GetText(RESOURCE_DIR "/Font/zpix.ttf", 20, "版本號 vN.N.N", Util::Color(255, 255, 255)));
	m_Version->SetZIndex(2);
	m_Version->m_Transform.translation = glm::vec2(450.0f, -300.0f);
}

void MainMenuScene::InitUIManager()
{
	UIManager::GetInstance().ResetPanels();

	auto panel = std::make_shared<SettingPanel>();
	panel->Start();
	UIManager::GetInstance().RegisterPanel("setting", panel);
}

void MainMenuScene::InitSettingButton()
{
	auto &img = ImagePoolManager::GetInstance();
	std::function<void()> onClick = []() { UIManager::GetInstance().ShowPanel("setting"); };
	m_SettingButton = std::make_shared<UIButton>(onClick, false);
	m_SettingButton->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_settingPanel/button_setting.png"));
	m_SettingButton->SetZIndex(3.0f);
	m_SettingButton->m_Transform.translation = {-550.0f, -280.0f};
}

void MainMenuScene::InitDeleteDataButton()
{
	auto &img = ImagePoolManager::GetInstance();
	std::function<void()> onClick = []()
	{
		auto &saveManager = SaveManager::GetInstance();
		if (saveManager.HasSaveData())
		{
			bool success = saveManager.DeleteSave();
			if (success)
			{
				LOG_INFO("Save data deleted successfully");
				AudioManager::GetInstance().PlaySFX("click");
			}
			else
			{
				LOG_ERROR("Failed to delete save data");
			}
		}
		else
		{
			LOG_INFO("No save data to delete");
		}
	};
	m_DeleteDataButton = std::make_shared<UIButton>(onClick, false);
	m_DeleteDataButton->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_menuHUD/button_deleteData.png"));
	m_DeleteDataButton->SetZIndex(3.0f);
	m_DeleteDataButton->m_Transform.translation = {-550.0f, -216.0f};
}

void MainMenuScene::InitAudioManager()
{
	AudioManager::GetInstance().Reset();
	AudioManager::GetInstance().LoadFromJson("/AudioConfig.json");
	AudioManager::GetInstance().PlayBGM("opening");
}

bool MainMenuScene::IsMouseClickingOnButtons() const
{
	// 檢查是否有設定面板顯示
	if (UIManager::GetInstance().IsPanelVisible("setting"))
	{
		return true; // 如果設定面板顯示，則阻擋輸入
	}

	// 獲取滑鼠座標
	glm::vec2 mousePos = Tool::GetMouseCoord();

	// 檢查是否點擊到設定按鈕
	if (m_SettingButton && m_SettingButton->IsVisible())
	{
		glm::vec2 buttonPos = m_SettingButton->m_Transform.translation;
		glm::vec2 buttonSize = m_SettingButton->GetScaledSize();

		if (mousePos.x >= buttonPos.x - buttonSize.x / 2.0f && mousePos.x <= buttonPos.x + buttonSize.x / 2.0f &&
			mousePos.y >= buttonPos.y - buttonSize.y / 2.0f && mousePos.y <= buttonPos.y + buttonSize.y / 2.0f)
		{
			return true;
		}
	}

	// 檢查是否點擊到刪除資料按鈕
	if (m_DeleteDataButton && m_DeleteDataButton->IsVisible())
	{
		glm::vec2 buttonPos = m_DeleteDataButton->m_Transform.translation;
		glm::vec2 buttonSize = m_DeleteDataButton->GetScaledSize();

		if (mousePos.x >= buttonPos.x - buttonSize.x / 2.0f && mousePos.x <= buttonPos.x + buttonSize.x / 2.0f &&
			mousePos.y >= buttonPos.y - buttonSize.y / 2.0f && mousePos.y <= buttonPos.y + buttonSize.y / 2.0f)
		{
			return true;
		}
	}

	// 檢查MenuHUDPanel按鈕
	if (m_MenuHUDPanel && m_MenuHUDPanel->IsMouseClickingOnButtons())
	{
		return true;
	}

	return false; // 沒有點擊到任何按鈕
}

// 初始化平移動畫計時器和回調函數
void MainMenuScene::InitSlideAnimation()
{
	// 設置平移動畫的更新回調
	m_SlideTimer.SetOnUpdate(
		[this](float progress)
		{
			// 使用緩動函數讓動畫更自然 (ease-out)
			const float easedProgress = Util::Timer::EaseOutQuad(progress);

			// 計算當前位置：從起始位置平移到目標位置
			const glm::vec2 currentPosition = m_StartPosition + (m_TargetPosition - m_StartPosition) * easedProgress;
			m_RedShawl->m_Transform.translation = currentPosition;
		});

	// 設置平移動畫完成時的回調
	m_SlideTimer.SetOnFinished([this]() { m_RedShawl->m_Transform.translation = m_TargetPosition; });
}

// 統一更新動畫
void MainMenuScene::UpdateAnimations() { m_SlideTimer.Update(); }

// 持續的文字閃爍效果（基於成員時間變數）
void MainMenuScene::UpdateTextBlinkEffect()
{
	const auto text = std::dynamic_pointer_cast<Util::Text>(m_Text->GetDrawable());
	if (!text)
		return;
	// 累積時間
	m_TextBlinkTime += Util::Time::GetDeltaTimeMs() / 1000.0f;

	// 每10秒重置時間並更新文字內容
	if (m_TextBlinkTime >= 10.0f)
		m_TextBlinkTime = 0.0f;
	else if (m_TextBlinkTime >= 7.5f)
		text->SetText("點擊呼叫司令部 >wOb");
	else if (m_TextBlinkTime >= 5.0f)
		text->SetText("點擊呼叫司令部 OwOb");
	else if (m_TextBlinkTime >= 2.5f)
		text->SetText("點擊呼叫司令部 Ow<b");
	else
		text->SetText("點擊呼叫司令部 OwOb");

	// 計算閃爍週期進度
	const float progress = fmod(m_TextBlinkTime, m_TextBlinkPeriod) / m_TextBlinkPeriod;

	// 使用正弦波來計算透明度
	const float alpha = std::max(std::abs(std::sin(static_cast<double>(progress) * M_PI * 2.0)), 0.1);

	// 計算透明度值
	const auto alphaValue = static_cast<uint8_t>(alpha * 255);

	// 創建新的顏色
	Util::Color newColor(255, 255, 255, alphaValue);

	// 更新文字顏色
	if (text)
	{
		text->SetColor(newColor);
	}
}

void MainMenuScene::StartSlideAnimation()
{
	m_RedShawl->SetVisible(true);

	// 設置起始位置（螢幕右邊外側）和目標位置
	m_TargetPosition = {237.0f, 22.0f}; // 目標的位置
	m_StartPosition = glm::vec2((static_cast<float>(PTSD_Config::WINDOW_WIDTH) + m_RedShawl->GetScaledSize().x) / 2,
								22.0f); // 螢幕右邊外側
	m_RedShawl->m_Transform.translation = m_StartPosition;

	// 開始平移動畫
	m_SlideTimer.Restart();
}

void MainMenuScene::InitMenuHUDPanel()
{
	m_MenuHUDPanel = std::make_shared<MenuHUDPanel>();
	m_MenuHUDPanel->Start();
}
