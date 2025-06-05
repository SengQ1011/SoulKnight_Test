//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/Result_Scene.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>


#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "SaveManager.hpp"
#include "Scene/SceneManager.hpp"
#include "Tool/Tool.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Text.hpp"
#include "Util/Time.hpp"
#include "config.hpp"
#include "imgui.h"


void ResultScene::Start()
{
	LOG_INFO("Entering Result Scene");

	// 處理遊戲數據 - 如果沒有數據就使用默認值
	Util::ms_t endTime, startTime;
	int killCount, timeScore, dungeonMoney, killScore, totalScore, gameMoney;

	if (!m_SceneData)
	{
		LOG_INFO("m_SceneData is null - using default test data for UI testing");
		// 使用默認測試數據
		endTime = 120;
		killCount = 15;
		dungeonMoney = 114;
		timeScore = std::max(0, 2000 - 2 * static_cast<int>(endTime));
		killScore = killCount * 2;
		totalScore = timeScore + killScore;
		gameMoney = totalScore / 100;

		// 設定默認關卡進度
		m_GameProgress.currentChapter = 1;
		m_GameProgress.currentStage = 4;
		m_GameProgress.killCount = killCount;
		m_GameProgress.cumulativeTime = endTime * 1000;

		m_PlayerData.playerID = 1;
		m_PlayerData.currentHp = 100;
		m_PlayerData.currentEnergy = 100;
		m_PlayerData.money = dungeonMoney;
	}
	else
	{
		// 使用真實遊戲數據
		m_GameProgress = m_SceneData->gameProgress;
		m_PlayerData = m_GameProgress.playerData;

		endTime = m_GameProgress.cumulativeTime / 1000;
		startTime = m_GameProgress.dungeonStartTime / 1000;
		killCount = m_GameProgress.killCount;
		dungeonMoney = m_PlayerData.money;
		timeScore = std::max(0, 500 - 2 * static_cast<int>(endTime - startTime));
		killScore = killCount * 2;
		totalScore = timeScore + killScore;
		gameMoney = totalScore / 100;

		// 保存進度
		m_SceneData->saveName = "Progress Save";
		m_SceneData->isInGameProgress = false;
		m_SceneData->gameData.gameMoney += gameMoney;
		Upload();
	}

	// 初始化所有UI元素
	InitResultBackground();
	InitBackground();
	InitGameEndTitle();
	InitProgressBar();
	InitEndIcon();
	InitStartIcon();
	InitPlayerIcon();
	InitLevelProgress();
	InitResultTemplate(endTime, killCount, dungeonMoney, gameMoney);
	InitResultIcons();
	InitContinueButton();
	InitButtonText();
	InitMagicStoneIcon();

	// 將所有元素添加到根節點
	m_Root->AddChild(m_Background);
	m_Root->AddChild(m_ResultGroupBackground);
	m_Root->AddChild(m_GameEndTitle);
	m_Root->AddChild(m_ProgressBar);
	m_Root->AddChild(m_EndIcon);
	m_Root->AddChild(m_StartIcon);
	m_Root->AddChild(m_PlayerIcon);
	m_Root->AddChild(m_LevelProgress);
	m_Root->AddChild(m_ClearTimeText);
	m_Root->AddChild(m_GoldText);
	m_Root->AddChild(m_GameCoinText);
	m_Root->AddChild(m_KillCountText);
	m_Root->AddChild(m_ClearTimeIcon);
	m_Root->AddChild(m_GoldIcon);
	m_Root->AddChild(m_GameCoinIcon);
	m_Root->AddChild(m_KillCountIcon);
	m_Root->AddChild(m_ContinueButton);
	m_Root->AddChild(m_ButtonText);
	m_Root->AddChild(m_MagicStoneIcon);

	FlushPendingObjectsToRendererAndCamera();
}

void ResultScene::Update()
{
	m_Root->Update();
	if (m_ContinueButton)
	{
		m_ContinueButton->Update();
	}

	// 讓 m_EndIcon 逆時針旋轉 - 簡單的持續旋轉
	if (m_EndIcon)
	{
		// 使用框架提供的精確時間計算
		float deltaTimeSeconds = Util::Time::GetDeltaTimeMs() / 1000.0f;

		// 簡單的逆時針旋轉
		float rotationSpeed = 2.0f; // 每秒旋轉2弧度
		m_EndIcon->m_Transform.rotation += rotationSpeed * deltaTimeSeconds;
	}

	// 更新玩家圖標移動
	UpdatePlayerIconMovement();

	// 顯示調試UI (按F1切換)
	if (Util::Input::IsKeyDown(Util::Keycode::F1))
	{
		m_ShowDebugUI = !m_ShowDebugUI;
	}

	if (m_ShowDebugUI)
	{
		DrawDebugUI();
	}
}

void ResultScene::Exit()
{
	LOG_DEBUG("Result Scene exited");
	// 重置關卡數據
	auto &sceneManager = SceneManager::GetInstance();
	sceneManager.ResetGameProgress();
}

Scene::SceneType ResultScene::Change()
{
	// 使用父類別的場景切換機制
	if (IsChange())
	{
		LOG_DEBUG("Change to Lobby");
		return Scene::SceneType::Lobby;
	}
	return Scene::SceneType::Null;
}

void ResultScene::InitBackground()
{
	try
	{
		auto &img = ImagePoolManager::GetInstance();
		auto drawable = img.GetImage(RESOURCE_DIR "/UI/ui_result/background_result.png");
		if (drawable && m_ResultGroupBackground)
		{
			m_ResultGroupBackground->SetDrawable(drawable);
			m_ResultGroupBackground->SetZIndex(0);
			m_ResultGroupBackground->m_Transform.translation = {-130, -50};
			LOG_DEBUG("Background initialized successfully");
		}
		else
		{
			LOG_ERROR("Failed to initialize background - drawable or m_Background is null");
		}
	}
	catch (const std::exception &e)
	{
		LOG_ERROR("Exception in InitBackground: {}", e.what());
	}
}

void ResultScene::InitGameEndTitle()
{
	auto &img = ImagePoolManager::GetInstance();
	m_GameEndTitle->SetDrawable(
		img.GetText(RESOURCE_DIR "/Font/zpix.ttf", 64, "游戲結束", Util::Color(255, 255, 255), false));
	m_GameEndTitle->SetZIndex(2);
	m_GameEndTitle->m_Transform.translation = {0, 300};
}

void ResultScene::InitProgressBar()
{
	auto &img = ImagePoolManager::GetInstance();
	m_ProgressBar->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/icon_process.png"));
	m_ProgressBar->SetZIndex(2);
	m_ProgressBar->m_Transform.translation = {0, 130};
}

void ResultScene::InitEndIcon()
{
	auto &img = ImagePoolManager::GetInstance();
	m_EndIcon->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/icon_end.png"));
	m_EndIcon->SetZIndex(2);
	m_EndIcon->m_Transform.translation = {390, 200};
}

void ResultScene::InitStartIcon()
{
	auto &img = ImagePoolManager::GetInstance();
	m_StartIcon->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/icon_start.png"));
	m_StartIcon->SetZIndex(2);
	m_StartIcon->m_Transform.translation = glm::vec2{-390, 200};
}

void ResultScene::InitLevelProgress()
{

	auto &img = ImagePoolManager::GetInstance();
	m_LevelProgress->SetDrawable(
		img.GetText(RESOURCE_DIR "/Font/zpix.ttf", 24, "1-1", Util::Color(255, 255, 255), false));
	m_LevelProgress->SetZIndex(2);

	// 通關文字對齊移動
	constexpr float offsetX = 9.0f;
	const auto offsetY = (m_ProgressBar->GetScaledSize().y + m_LevelProgress->GetScaledSize().y) / 2.0f + 3.0f;
	m_LevelProgress->m_Transform.translation.x = m_PlayerIcon->m_Transform.translation.x + offsetX;
	m_LevelProgress->m_Transform.translation.y = m_ProgressBar->m_Transform.translation.y - offsetY;
}

void ResultScene::InitResultTemplate(int totalSecond, int killCount, int dungeonMoney, int gameMoney)
{
	auto &img = ImagePoolManager::GetInstance();

	// 格式化時間顯示為 H:MM:SS 格式
	std::stringstream timeStream;
	int hours = totalSecond / 3600;
	int minutes = (totalSecond % 3600) / 60;
	int seconds = totalSecond % 60;
	timeStream << hours << ":" << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2) << seconds;

	// 通關時間 - 直接顯示時間數據
	m_ClearTimeText->SetDrawable(
		img.GetText(RESOURCE_DIR "/Font/pixel_bold.ttf", 32, timeStream.str(), Util::Color(255, 255, 255), false));
	m_ClearTimeText->SetZIndex(2);
	m_ClearTimeText->m_Transform.translation =
		m_ResultGroupBackground->m_Transform.translation + CLEAR_TIME_TEXT_OFFSET;

	// 金幣 - 直接顯示數字
	m_GoldText->SetDrawable(img.GetText(RESOURCE_DIR "/Font/pixel_bold.ttf", 32, std::to_string(dungeonMoney),
										Util::Color(255, 215, 0), false));
	m_GoldText->SetZIndex(2);
	m_GoldText->m_Transform.translation = m_ResultGroupBackground->m_Transform.translation + GOLD_TEXT_OFFSET;

	// 遊戲幣 - 格式為 +數字
	m_GameCoinText->SetDrawable(img.GetText(RESOURCE_DIR "/Font/pixel_bold.ttf", 32, "+" + std::to_string(gameMoney),
											Util::Color(0, 255, 255), false));
	m_GameCoinText->SetZIndex(2);
	m_GameCoinText->m_Transform.translation = m_ResultGroupBackground->m_Transform.translation + GAME_COIN_TEXT_OFFSET;

	// 殺敵數 - 直接顯示數字
	m_KillCountText->SetDrawable(img.GetText(RESOURCE_DIR "/Font/pixel_bold.ttf", 32, std::to_string(killCount),
											 Util::Color(255, 100, 100), false));
	m_KillCountText->SetZIndex(2);
	m_KillCountText->m_Transform.translation =
		m_ResultGroupBackground->m_Transform.translation + KILL_COUNT_TEXT_OFFSET;
}

void ResultScene::InitContinueButton()
{
	auto &img = ImagePoolManager::GetInstance();

	// 創建功能一的回調函數：開始玩家圖標移動
	auto onClick = [this]()
	{
		LOG_DEBUG("Continue button clicked - starting player movement");
		AudioManager::GetInstance().PlaySFX("click");
		StartPlayerMovement();
	};

	m_ContinueButton = std::make_shared<UIButton>(onClick, false);
	m_ContinueButton->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/button_continue.png"));
	m_ContinueButton->SetZIndex(3.0f);
	m_ContinueButton->m_Transform.translation = {0, -250};
}

void ResultScene::InitMagicStoneIcon()
{
	auto &img = ImagePoolManager::GetInstance();
	m_MagicStoneIcon->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/icon_magicCube.png"));
	m_MagicStoneIcon->SetZIndex(2);
	m_MagicStoneIcon->m_Transform.translation = {280, -50};
}

void ResultScene::InitResultIcons()
{
	auto &img = ImagePoolManager::GetInstance();

	// 通關時間圖標
	m_ClearTimeIcon->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/icon_time.png"));
	m_ClearTimeIcon->SetZIndex(2);
	m_ClearTimeIcon->m_Transform.translation =
		m_ResultGroupBackground->m_Transform.translation + CLEAR_TIME_ICON_OFFSET;

	// 金幣圖標
	m_GoldIcon->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/icon_coin.png"));
	m_GoldIcon->SetZIndex(2);
	m_GoldIcon->m_Transform.translation = m_ResultGroupBackground->m_Transform.translation + GOLD_ICON_OFFSET;

	// 遊戲幣圖標
	m_GameCoinIcon->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/icon_gameCoin.png"));
	m_GameCoinIcon->SetZIndex(2);
	m_GameCoinIcon->m_Transform.translation = m_ResultGroupBackground->m_Transform.translation + GAME_COIN_ICON_OFFSET;
	m_GameCoinIcon->m_Transform.scale = glm::vec2(3.556f);

	// 殺敵數圖標
	m_KillCountIcon->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/icon_killCount.png"));
	m_KillCountIcon->SetZIndex(2);
	m_KillCountIcon->m_Transform.translation =
		m_ResultGroupBackground->m_Transform.translation + KILL_COUNT_ICON_OFFSET;
}

void ResultScene::InitResultBackground()
{
	auto &img = ImagePoolManager::GetInstance();
	// 隨意放一個背景圖片，您可以稍後更改
	m_Background->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/background_scene.png"));
	m_Background->SetZIndex(-1); // 設為最底層
	m_Background->m_Transform.scale = glm::vec2(1.0f, 0.63f);
}

void ResultScene::InitPlayerIcon()
{
	auto &img = ImagePoolManager::GetInstance();
	// 使用騎士圖標，位置與 StartIcon 相同
	m_PlayerIcon->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/icon_knight.png"));
	m_PlayerIcon->SetZIndex(3);
	m_PlayerIcon->m_Transform.translation = glm::vec2{-390, 200}; // 與 m_StartIcon 相同位置
	m_PlayerIcon->m_Transform.scale = glm::vec2(4.0f); // 適當縮放
}

void ResultScene::InitButtonText()
{
	auto &img = ImagePoolManager::GetInstance();
	m_ButtonText->SetDrawable(
		std::make_shared<Util::Text>(RESOURCE_DIR "/Font/zpix.ttf", 32, "開始結算", Util::Color(255, 255, 255), false));
	m_ButtonText->SetZIndex(4); // 比按鈕高一層
	m_ButtonText->m_Transform.translation = {15, -250}; // 跟隨 m_ContinueButton 的位置
}

void ResultScene::StartPlayerMovement()
{
	if (m_IsPlayerMoving || m_HasReachedEnd)
		return;

	// 計算目標位置
	float startX = m_StartIcon->m_Transform.translation.x;
	float endX = m_EndIcon->m_Transform.translation.x;
	float totalDistance = endX - startX;

	// 根據關卡進度計算目標位置
	int totalStages = m_GameProgress.currentStage + (m_GameProgress.currentChapter - 1) * 5;
	float targetX = startX + (totalStages * totalDistance / 5.0f);

	m_PlayerStartPosition = m_PlayerIcon->m_Transform.translation;
	m_PlayerTargetPosition = {targetX, m_PlayerIcon->m_Transform.translation.y};
	m_IsPlayerMoving = true;
}

void ResultScene::UpdatePlayerIconMovement()
{
	if (!m_IsPlayerMoving)
		return;

	float deltaTimeSeconds = Util::Time::GetDeltaTimeMs() / 1000.0f;

	// 計算移動方向和距離
	glm::vec2 direction = m_PlayerTargetPosition - m_PlayerIcon->m_Transform.translation;
	float distance = glm::length(direction);

	if (distance < 1.0f) // 到達目標位置
	{
		m_PlayerIcon->m_Transform.translation = m_PlayerTargetPosition;
		m_IsPlayerMoving = false;
		m_HasReachedEnd = true;

		// 切換按鈕功能
		SwitchToReturnLobbyMode();
	}
	else
	{
		// 繼續移動
		glm::vec2 velocity = glm::normalize(direction) * PLAYER_MOVE_SPEED;
		m_PlayerIcon->m_Transform.translation += velocity * deltaTimeSeconds;

		// 通關文字對齊移動
		constexpr float offsetX = 9.0f;
		const auto offsetY = (m_ProgressBar->GetScaledSize().y + m_LevelProgress->GetScaledSize().y) / 2.0f + 3.0f;
		m_LevelProgress->m_Transform.translation.x = m_PlayerIcon->m_Transform.translation.x + offsetX;
		m_LevelProgress->m_Transform.translation.y = m_ProgressBar->m_Transform.translation.y - offsetY;

		// 檢查是否經過新的段落，更新關卡進度文字
		float startX = m_StartIcon->m_Transform.translation.x;
		float endX = m_EndIcon->m_Transform.translation.x;
		float currentProgress = (m_PlayerIcon->m_Transform.translation.x - startX) / (endX - startX);

		// 修改計算邏輯：確保只有完全通過階段才顯示下一階段
		// 將進度條分為5段，每段代表一個關卡
		// 只有當進度 >= 某段的結束點時，才顯示該段已完成
		int completedStages = static_cast<int>(currentProgress * 5.0f);

		// 確保 completedStages 在有效範圍內 (0-4)，對應關卡 1-1 到 1-5
		completedStages = std::max(0, std::min(4, completedStages));

		// 顯示已完成的關卡數 + 1（因為關卡從1開始）
		int displayStage = completedStages + 1;
		int displayChapter = 1; // 目前只有第一章

		std::string progressText = std::to_string(displayChapter) + "-" + std::to_string(displayStage);
		const auto text = std::dynamic_pointer_cast<Util::Text>(m_LevelProgress->GetDrawable());
		text->SetText(progressText);
	}
}


void ResultScene::SwitchToReturnLobbyMode()
{
	// 更新按鈕文字
	const auto text = std::dynamic_pointer_cast<Util::Text>(m_ButtonText->GetDrawable());
	if (text)
	{
		text->SetText("返回大廳");
	}
	else
	{
		LOG_ERROR("ResultScene::SwitchToReturnLobbyMode() ERROR");
	}

	// 創建功能二的回調函數：返回Lobby
	auto onClick = [this]()
	{
		LOG_DEBUG("Return to Lobby button clicked");
		AudioManager::GetInstance().PlaySFX("click");
		SetIsChange(true); // 使用父類別的場景切換機制
	};

	// 重新設置按鈕回調
	auto &img = ImagePoolManager::GetInstance();
	m_ContinueButton = std::make_shared<UIButton>(onClick, false);
	m_ContinueButton->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_result/button_continue.png"));
	m_ContinueButton->SetZIndex(3.0f);
	m_ContinueButton->m_Transform.translation = {0, -250};
}

void ResultScene::DrawDebugUI()
{
	// 創建主調試視窗
	ImGui::Begin("Result Scene Debug UI");

	ImGui::Text("Press F1 to toggle debug UI");
	ImGui::Separator();

	// 背景調試
	if (ImGui::CollapsingHeader("Background"))
	{
		if (m_ResultGroupBackground && m_ResultGroupBackground->GetDrawable())
		{
			float pos[2] = {m_ResultGroupBackground->m_Transform.translation.x,
							m_ResultGroupBackground->m_Transform.translation.y};
			glm::vec2 currentSize =
				m_ResultGroupBackground->m_Transform.scale * m_ResultGroupBackground->GetImageSize();
			float size[2] = {currentSize.x, currentSize.y};

			if (ImGui::DragFloat2("Position##bg", pos, 1.0f))
				m_ResultGroupBackground->m_Transform.translation = {pos[0], pos[1]};

			if (ImGui::DragFloat2("Size (pixels)##bg", size, 1.0f, 1.0f, 2000.0f))
				m_ResultGroupBackground->m_Transform.scale =
					glm::vec2(size[0], size[1]) / m_ResultGroupBackground->GetImageSize();
		}
	}

	// Game End Title
	if (ImGui::CollapsingHeader("Game End Title"))
	{
		if (m_GameEndTitle && m_GameEndTitle->GetDrawable())
		{
			float pos[2] = {m_GameEndTitle->m_Transform.translation.x, m_GameEndTitle->m_Transform.translation.y};
			glm::vec2 currentSize = m_GameEndTitle->m_Transform.scale * m_GameEndTitle->GetImageSize();
			float size[2] = {currentSize.x, currentSize.y};

			if (ImGui::DragFloat2("Position##title", pos, 1.0f))
				m_GameEndTitle->m_Transform.translation = {pos[0], pos[1]};

			if (ImGui::DragFloat2("Size (pixels)##title", size, 1.0f, 1.0f, 2000.0f))
				m_GameEndTitle->m_Transform.scale = glm::vec2(size[0], size[1]) / m_GameEndTitle->GetImageSize();
		}
	}

	// Progress Bar
	if (ImGui::CollapsingHeader("Progress Bar"))
	{
		if (m_ProgressBar && m_ProgressBar->GetDrawable())
		{
			float pos[2] = {m_ProgressBar->m_Transform.translation.x, m_ProgressBar->m_Transform.translation.y};
			glm::vec2 currentSize = m_ProgressBar->m_Transform.scale * m_ProgressBar->GetImageSize();
			float size[2] = {currentSize.x, currentSize.y};

			if (ImGui::DragFloat2("Position##progress", pos, 1.0f))
				m_ProgressBar->m_Transform.translation = {pos[0], pos[1]};

			if (ImGui::DragFloat2("Size (pixels)##progress", size, 1.0f, 1.0f, 2000.0f))
				m_ProgressBar->m_Transform.scale = glm::vec2(size[0], size[1]) / m_ProgressBar->GetImageSize();
		}
	}

	// Magic Cube
	if (ImGui::CollapsingHeader("Magic Cube"))
	{
		if (m_EndIcon && m_EndIcon->GetDrawable())
		{
			float pos[2] = {m_EndIcon->m_Transform.translation.x, m_EndIcon->m_Transform.translation.y};
			glm::vec2 currentSize = m_EndIcon->m_Transform.scale * m_EndIcon->GetImageSize();
			float size[2] = {currentSize.x, currentSize.y};

			if (ImGui::DragFloat2("Position##cube", pos, 1.0f))
				m_EndIcon->m_Transform.translation = {pos[0], pos[1]};

			if (ImGui::DragFloat2("Size (pixels)##cube", size, 1.0f, 1.0f, 2000.0f))
				m_EndIcon->m_Transform.scale = glm::vec2(size[0], size[1]) / m_EndIcon->GetImageSize();
		}
	}

	// Character Icon
	if (ImGui::CollapsingHeader("Character Icon"))
	{
		if (m_StartIcon && m_StartIcon->GetDrawable())
		{
			float pos[2] = {m_StartIcon->m_Transform.translation.x, m_StartIcon->m_Transform.translation.y};
			glm::vec2 currentSize = m_StartIcon->m_Transform.scale * m_StartIcon->GetImageSize();
			float size[2] = {currentSize.x, currentSize.y};

			if (ImGui::DragFloat2("Position##char", pos, 1.0f))
				m_StartIcon->m_Transform.translation = {pos[0], pos[1]};

			if (ImGui::DragFloat2("Size (pixels)##char", size, 1.0f, 1.0f, 2000.0f))
				m_StartIcon->m_Transform.scale = glm::vec2(size[0], size[1]) / m_StartIcon->GetImageSize();
		}
	}

	// Result Texts
	if (ImGui::CollapsingHeader("Result Texts"))
	{
		// Clear Time Text
		if (m_ClearTimeText && m_ClearTimeText->GetDrawable())
		{
			glm::vec2 relativePos =
				m_ClearTimeText->m_Transform.translation - m_ResultGroupBackground->m_Transform.translation;
			float pos[2] = {relativePos.x, relativePos.y};
			if (ImGui::DragFloat2("Clear Time Offset", pos, 1.0f))
				m_ClearTimeText->m_Transform.translation =
					m_ResultGroupBackground->m_Transform.translation + glm::vec2(pos[0], pos[1]);
		}

		// Gold Text
		if (m_GoldText && m_GoldText->GetDrawable())
		{
			glm::vec2 relativePos =
				m_GoldText->m_Transform.translation - m_ResultGroupBackground->m_Transform.translation;
			float pos[2] = {relativePos.x, relativePos.y};
			if (ImGui::DragFloat2("Gold Offset", pos, 1.0f))
				m_GoldText->m_Transform.translation =
					m_ResultGroupBackground->m_Transform.translation + glm::vec2(pos[0], pos[1]);
		}

		// Game Coin Text
		if (m_GameCoinText && m_GameCoinText->GetDrawable())
		{
			glm::vec2 relativePos =
				m_GameCoinText->m_Transform.translation - m_ResultGroupBackground->m_Transform.translation;
			float pos[2] = {relativePos.x, relativePos.y};
			if (ImGui::DragFloat2("Game Coin Offset", pos, 1.0f))
				m_GameCoinText->m_Transform.translation =
					m_ResultGroupBackground->m_Transform.translation + glm::vec2(pos[0], pos[1]);
		}

		// Kill Count Text
		if (m_KillCountText && m_KillCountText->GetDrawable())
		{
			glm::vec2 relativePos =
				m_KillCountText->m_Transform.translation - m_ResultGroupBackground->m_Transform.translation;
			float pos[2] = {relativePos.x, relativePos.y};
			if (ImGui::DragFloat2("Kill Count Offset", pos, 1.0f))
				m_KillCountText->m_Transform.translation =
					m_ResultGroupBackground->m_Transform.translation + glm::vec2(pos[0], pos[1]);
		}
	}

	// Result Icons
	if (ImGui::CollapsingHeader("Result Icons"))
	{
		// Clear Time Icon
		if (m_ClearTimeIcon && m_ClearTimeIcon->GetDrawable())
		{
			glm::vec2 relativePos =
				m_ClearTimeIcon->m_Transform.translation - m_ResultGroupBackground->m_Transform.translation;
			float pos[2] = {relativePos.x, relativePos.y};
			glm::vec2 currentSize = m_ClearTimeIcon->m_Transform.scale * m_ClearTimeIcon->GetImageSize();
			float size[2] = {currentSize.x, currentSize.y};

			if (ImGui::DragFloat2("Clear Time Icon Offset", pos, 1.0f))
				m_ClearTimeIcon->m_Transform.translation =
					m_ResultGroupBackground->m_Transform.translation + glm::vec2(pos[0], pos[1]);

			if (ImGui::DragFloat2("Clear Time Icon Size", size, 1.0f, 1.0f, 1000.0f))
				m_ClearTimeIcon->m_Transform.scale = glm::vec2(size[0], size[1]) / m_ClearTimeIcon->GetImageSize();
		}

		// Gold Icon
		if (m_GoldIcon && m_GoldIcon->GetDrawable())
		{
			glm::vec2 relativePos =
				m_GoldIcon->m_Transform.translation - m_ResultGroupBackground->m_Transform.translation;
			float pos[2] = {relativePos.x, relativePos.y};
			glm::vec2 currentSize = m_GoldIcon->m_Transform.scale * m_GoldIcon->GetImageSize();
			float size[2] = {currentSize.x, currentSize.y};

			if (ImGui::DragFloat2("Gold Icon Offset", pos, 1.0f))
				m_GoldIcon->m_Transform.translation =
					m_ResultGroupBackground->m_Transform.translation + glm::vec2(pos[0], pos[1]);

			if (ImGui::DragFloat2("Gold Icon Size", size, 1.0f, 1.0f, 1000.0f))
				m_GoldIcon->m_Transform.scale = glm::vec2(size[0], size[1]) / m_GoldIcon->GetImageSize();
		}

		// Game Coin Icon
		if (m_GameCoinIcon && m_GameCoinIcon->GetDrawable())
		{
			glm::vec2 relativePos =
				m_GameCoinIcon->m_Transform.translation - m_ResultGroupBackground->m_Transform.translation;
			float pos[2] = {relativePos.x, relativePos.y};
			glm::vec2 currentSize = m_GameCoinIcon->m_Transform.scale * m_GameCoinIcon->GetImageSize();
			float size[2] = {currentSize.x, currentSize.y};

			if (ImGui::DragFloat2("Game Coin Icon Offset", pos, 1.0f))
				m_GameCoinIcon->m_Transform.translation =
					m_ResultGroupBackground->m_Transform.translation + glm::vec2(pos[0], pos[1]);

			if (ImGui::DragFloat2("Game Coin Icon Size", size, 1.0f, 1.0f, 1000.0f))
				m_GameCoinIcon->m_Transform.scale = glm::vec2(size[0], size[1]) / m_GameCoinIcon->GetImageSize();
		}

		// Kill Count Icon
		if (m_KillCountIcon && m_KillCountIcon->GetDrawable())
		{
			glm::vec2 relativePos =
				m_KillCountIcon->m_Transform.translation - m_ResultGroupBackground->m_Transform.translation;
			float pos[2] = {relativePos.x, relativePos.y};
			glm::vec2 currentSize = m_KillCountIcon->m_Transform.scale * m_KillCountIcon->GetImageSize();
			float size[2] = {currentSize.x, currentSize.y};

			if (ImGui::DragFloat2("Kill Count Icon Offset", pos, 1.0f))
				m_KillCountIcon->m_Transform.translation =
					m_ResultGroupBackground->m_Transform.translation + glm::vec2(pos[0], pos[1]);

			if (ImGui::DragFloat2("Kill Count Icon Size", size, 1.0f, 1.0f, 1000.0f))
				m_KillCountIcon->m_Transform.scale = glm::vec2(size[0], size[1]) / m_KillCountIcon->GetImageSize();
		}
	}

	// Background
	if (ImGui::CollapsingHeader("Additional Background"))
	{
		if (m_Background && m_Background->GetDrawable())
		{
			float pos[2] = {m_Background->m_Transform.translation.x, m_Background->m_Transform.translation.y};
			glm::vec2 currentSize = m_Background->m_Transform.scale * m_Background->GetImageSize();
			float size[2] = {currentSize.x, currentSize.y};

			if (ImGui::DragFloat2("Additional Background Position", pos, 1.0f))
				m_Background->m_Transform.translation = {pos[0], pos[1]};

			if (ImGui::DragFloat2("Additional Background Size", size, 1.0f, 1.0f, 2000.0f))
				m_Background->m_Transform.scale = glm::vec2(size[0], size[1]) / m_Background->GetImageSize();
		}
	}

	// Magic Stone Icon
	if (ImGui::CollapsingHeader("Magic Stone Icon"))
	{
		if (m_MagicStoneIcon && m_MagicStoneIcon->GetDrawable())
		{
			float pos[2] = {m_MagicStoneIcon->m_Transform.translation.x, m_MagicStoneIcon->m_Transform.translation.y};
			glm::vec2 currentSize = m_MagicStoneIcon->m_Transform.scale * m_MagicStoneIcon->GetImageSize();
			float size[2] = {currentSize.x, currentSize.y};

			if (ImGui::DragFloat2("Position##stone", pos, 1.0f))
				m_MagicStoneIcon->m_Transform.translation = {pos[0], pos[1]};

			if (ImGui::DragFloat2("Size (pixels)##stone", size, 1.0f, 1.0f, 1000.0f))
				m_MagicStoneIcon->m_Transform.scale = glm::vec2(size[0], size[1]) / m_MagicStoneIcon->GetImageSize();
		}
	}

	// Button Text
	if (ImGui::CollapsingHeader("Button Text"))
	{
		if (m_ButtonText && m_ButtonText->GetDrawable())
		{
			float pos[2] = {m_ButtonText->m_Transform.translation.x, m_ButtonText->m_Transform.translation.y};

			if (ImGui::DragFloat2("Position##buttontext", pos, 1.0f))
				m_ButtonText->m_Transform.translation = {pos[0], pos[1]};
		}
	}

	ImGui::End();

	// Continue Button Debug UI
	if (m_ContinueButton && Util::Input::IsKeyDown(Util::Keycode::F1))
	{
		m_ContinueButton->DrawDebugUI();
	}
}
