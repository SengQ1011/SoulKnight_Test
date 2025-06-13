#include "UIPanel/GameHUDPanel.hpp"

#include "Components/walletComponent.hpp"
#include "ImagePoolManager.hpp"
#include "Override/nGameObject.hpp"
#include "Room/DungeonMap.hpp"
#include "SaveManager.hpp"
#include "Scene/SceneManager.hpp"
#include "UIPanel/MinimapPanel.hpp"
#include "UIPanel/PlayerStatusPanel.hpp"
#include "UIPanel/UIButton.hpp"
#include "UIPanel/UIManager.hpp"
#include "Util/Color.hpp"
#include "Util/Input.hpp"
#include "Util/Time.hpp"
#include "config.hpp"


// 暫停按鈕回調函數
void OpenPausePanelFromHUD() { UIManager::GetInstance().ShowPanel("pause"); }

GameHUDPanel::GameHUDPanel(const std::shared_ptr<HealthComponent> &healthComp,
						   const std::shared_ptr<walletComponent> &walletComp,
						   const std::shared_ptr<DungeonMap> &dungeonMap) :
	m_PlayerHealthComponent(healthComp), m_PlayerWalletComponent(walletComp), m_DungeonMap(dungeonMap)
{
}

void GameHUDPanel::Start()
{
	// 設置面板名稱
	SetPanelName("gameHUD");

	// 初始化玩家狀態面板
	m_PlayerStatusPanel = std::make_shared<PlayerStatusPanel>(m_PlayerHealthComponent);
	m_PlayerStatusPanel->Start();

	// 初始化小地圖面板（如果有DungeonMap的話）
	if (m_DungeonMap)
	{
		m_MinimapPanel = std::make_shared<MinimapPanel>(m_DungeonMap);
		m_MinimapPanel->Start();
	}

	// 初始化暫停按鈕
	InitializePauseButton();

	// 初始化金幣顯示
	InitializeCoinDisplay();

	// 初始化遊戲幣顯示
	InitializeGameCoinDisplay();

	// 將所有元素添加到場景
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		const auto renderer = scene->GetRoot().lock();
		renderer->AddChild(std::static_pointer_cast<nGameObject>(m_PauseButton));
		renderer->AddChild(m_CoinBackground);
		renderer->AddChild(m_CoinText);
		renderer->AddChild(m_GameCoinBackground);
		renderer->AddChild(m_GameCoinText);
	}

	// 默認顯示
	Show();
}

void GameHUDPanel::Update()
{
	// 檢查是否應該阻擋輸入
	bool blockInput = ShouldBlockInput();

	// 更新動畫
	UpdateAnimation();

	// 更新金幣顯示
	UpdateCoinDisplay();

	// 更新遊戲幣顯示
	UpdateGameCoinDisplay();

	// 更新子面板
	if (m_PlayerStatusPanel)
		m_PlayerStatusPanel->Update();

	// 更新小地圖面板
	if (m_MinimapPanel)
		m_MinimapPanel->Update();

	// 更新按鈕（考慮輸入阻擋）
	if (m_PauseButton)
	{
		if (blockInput)
		{
			std::static_pointer_cast<nGameObject>(m_PauseButton)->SetActive(false);
			m_PauseButton->Update();
			std::static_pointer_cast<nGameObject>(m_PauseButton)->SetActive(true);
		}
		else
		{
			m_PauseButton->Update();
		}
	}

	// 更新所有遊戲對象
	for (const auto &gameObject : m_GameObjects)
	{
		if (gameObject)
			gameObject->Update();
	}

	// 調試界面
	if (Util::Input::IsKeyDown(Util::Keycode::F1))
	{
		DrawDebugUI();
	}
}

void GameHUDPanel::Show()
{
	if (m_IsAnimating && m_IsShowingAnimation)
		return;

	UIPanel::Show();

	// 顯示子面板
	if (m_PlayerStatusPanel)
		std::static_pointer_cast<UIPanel>(m_PlayerStatusPanel)->Show();

	// 顯示小地圖面板
	if (m_MinimapPanel)
		m_MinimapPanel->Show();

	StartShowAnimation();
}

void GameHUDPanel::Hide()
{
	if (m_IsAnimating && !m_IsShowingAnimation)
		return;

	// 隱藏子面板
	if (m_PlayerStatusPanel)
		std::static_pointer_cast<UIPanel>(m_PlayerStatusPanel)->Hide();

	// 隱藏小地圖面板
	if (m_MinimapPanel)
		m_MinimapPanel->Hide();

	StartHideAnimation();
}

void GameHUDPanel::InitializePauseButton()
{
	auto &img = ImagePoolManager::GetInstance();

	m_PauseButton = std::make_shared<UIButton>(OpenPausePanelFromHUD, false);
	std::static_pointer_cast<nGameObject>(m_PauseButton)
		->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_pausePanel/button_pause.png"));
	std::static_pointer_cast<nGameObject>(m_PauseButton)->SetZIndex(85.0f);
	m_PauseButton->m_Transform.translation = m_PauseButtonVisiblePos;

	m_GameObjects.push_back(std::static_pointer_cast<nGameObject>(m_PauseButton));
}

void GameHUDPanel::InitializeCoinDisplay()
{
	auto &img = ImagePoolManager::GetInstance();

	// 創建金幣背景
	m_CoinBackground = std::make_shared<nGameObject>();
	m_CoinBackground->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_HUD/show_coin.png"));
	m_CoinBackground->SetZIndex(85.0f);
	m_CoinBackground->m_Transform.translation = m_CoinBackgroundPos;
	m_GameObjects.push_back(m_CoinBackground);

	// 創建金幣文字
	m_CoinText = std::make_shared<nGameObject>();
	m_CoinText->SetZIndex(m_CoinBackground->GetZIndex() + 0.1f); // 比背景高一層
	m_CoinText->SetZIndexType(CUSTOM);

	// 初始化文字內容
	UpdateCoinDisplay();

	m_GameObjects.push_back(m_CoinText);
}

void GameHUDPanel::InitializeGameCoinDisplay()
{
	auto &img = ImagePoolManager::GetInstance();

	// 創建遊戲幣背景
	m_GameCoinBackground = std::make_shared<nGameObject>();
	m_GameCoinBackground->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_HUD/show_gameCoin.png"));
	m_GameCoinBackground->SetZIndex(85.0f);

	// 計算遊戲幣背景位置（在金幣背景左側）
	float offset = (m_CoinBackground->GetScaledSize().x + m_GameCoinBackground->GetScaledSize().x) / 2.0f + 16.0f;
	m_GameCoinBackground->m_Transform.translation = m_CoinBackground->m_Transform.translation - glm::vec2(offset, 0.0f);

	m_GameObjects.push_back(m_GameCoinBackground);

	// 創建遊戲幣文字
	m_GameCoinText = std::make_shared<nGameObject>();
	m_GameCoinText->SetZIndex(m_GameCoinBackground->GetZIndex() + 0.1f); // 比背景高一層
	m_GameCoinText->SetZIndexType(CUSTOM);

	// 初始化文字內容
	UpdateGameCoinDisplay();

	m_GameObjects.push_back(m_GameCoinText);
}

void GameHUDPanel::UpdateCoinDisplay()
{
	if (!m_PlayerWalletComponent || !m_CoinText)
		return;

	int currentCoins = m_PlayerWalletComponent->GetMoney();

	// 只有金幣數量改變時才更新文字（性能優化）
	if (currentCoins != m_LastCoinAmount)
	{
		m_LastCoinAmount = currentCoins;

		// 更新文字內容
		std::string coinText = std::to_string(currentCoins);
		m_CoinText->SetDrawable(ImagePoolManager::GetInstance().GetText(RESOURCE_DIR "/Font/zpix.TTF", 24, coinText,
																		Util::Color(255, 255, 255), false));

		// 更新文字位置（相對於背景位置 + 偏移）
		m_CoinText->m_Transform.translation = m_CoinBackgroundPos + m_CoinTextOffset;
	}
}

void GameHUDPanel::UpdateGameCoinDisplay()
{
	if (!m_GameCoinText)
		return;

	// 從場景數據中獲取遊戲幣數量
	int currentGameCoins = 0;
	if (const auto scene = SceneManager::GetInstance().GetCurrentScene().lock())
	{
		if (auto saveData = scene->GetSaveData())
		{
			currentGameCoins = saveData->gameData.gameMoney;
		}
	}

	// 只有遊戲幣數量改變時才更新文字（性能優化）
	if (currentGameCoins != m_LastGameCoinAmount)
	{
		m_LastGameCoinAmount = currentGameCoins;

		// 更新文字內容
		std::string gameCoinText = std::to_string(currentGameCoins);
		m_GameCoinText->SetDrawable(ImagePoolManager::GetInstance().GetText(
			RESOURCE_DIR "/Font/zpix.TTF", 24, gameCoinText, Util::Color(255, 255, 255), false));

		// 更新文字位置（相對於背景位置 + 偏移）
		m_GameCoinText->m_Transform.translation = m_GameCoinBackground->m_Transform.translation + m_GameCoinTextOffset;
	}
}

void GameHUDPanel::StartShowAnimation()
{
	m_IsAnimating = true;
	m_IsShowingAnimation = true;
	m_AnimationTimer = 0.0f;
}

void GameHUDPanel::StartHideAnimation()
{
	m_IsAnimating = true;
	m_IsShowingAnimation = false;
	m_AnimationTimer = 0.0f;
}

void GameHUDPanel::UpdateAnimation()
{
	if (!m_IsAnimating)
		return;

	m_AnimationTimer += Util::Time::GetDeltaTimeMs() / 1000.0f;
	const float progress = std::min(m_AnimationTimer / m_AnimationDuration, 1.0f);

	UpdateElementsPosition(progress);

	// 動畫完成
	if (progress >= 1.0f)
	{
		m_IsAnimating = false;

		if (!m_IsShowingAnimation)
		{
			// Hide 動畫完成，真正隱藏面板
			UIPanel::Hide();
		}
	}
}

void GameHUDPanel::UpdateElementsPosition(float progress)
{
	const float easedProgress = EaseOutQuad(progress);

	glm::vec2 pauseButtonPos;
	if (m_IsShowingAnimation)
	{
		// Show: 從隱藏位置滑入
		pauseButtonPos = m_PauseButtonHiddenPos + (m_PauseButtonVisiblePos - m_PauseButtonHiddenPos) * easedProgress;
	}
	else
	{
		// Hide: 滑出到隱藏位置
		pauseButtonPos = m_PauseButtonVisiblePos + (m_PauseButtonHiddenPos - m_PauseButtonVisiblePos) * easedProgress;
	}

	if (m_PauseButton)
		m_PauseButton->m_Transform.translation = pauseButtonPos;
}

void GameHUDPanel::DrawDebugUI()
{
	ImGui::Begin("GameHUD Debug");

	// === 金幣顯示設定 ===
	if (ImGui::CollapsingHeader("Coin Display Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static bool posChanged = false;
		static bool offsetChanged = false;

		// 背景位置設定
		ImGui::Text("Coin Background Position:");
		ImGui::InputFloat("Background X", &m_CoinBackgroundPos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Background Y", &m_CoinBackgroundPos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;

		// 文字偏移設定
		ImGui::Text("Text Offset (relative to background):");
		ImGui::InputFloat("Text Offset X", &m_CoinTextOffset.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			offsetChanged = true;
		ImGui::InputFloat("Text Offset Y", &m_CoinTextOffset.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			offsetChanged = true;

		// 即時更新位置
		if (posChanged || offsetChanged)
		{
			if (m_CoinBackground)
				m_CoinBackground->m_Transform.translation = m_CoinBackgroundPos;
			if (m_CoinText)
				m_CoinText->m_Transform.translation = m_CoinBackgroundPos + m_CoinTextOffset;
			posChanged = false;
			offsetChanged = false;
		}

		// 顯示當前金幣數量
		if (m_PlayerWalletComponent)
		{
			ImGui::Text("Current Coins: %d", m_PlayerWalletComponent->GetMoney());
		}

		// 測試按鈕：增加/減少金幣
		if (ImGui::Button("Add 100 Coins"))
		{
			if (m_PlayerWalletComponent)
				m_PlayerWalletComponent->AddMoney(100);
		}
		ImGui::SameLine();
		if (ImGui::Button("Spend 50 Coins"))
		{
			if (m_PlayerWalletComponent)
				m_PlayerWalletComponent->SpendMoney(50);
		}
	}

	// === 遊戲幣顯示設定 ===
	if (ImGui::CollapsingHeader("Game Coin Display Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static bool gameCoinPosChanged = false;
		static bool gameCoinOffsetChanged = false;

		// 背景位置設定
		ImGui::Text("Game Coin Background Position:");
		ImGui::InputFloat("Background X", &m_GameCoinBackgroundPos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			gameCoinPosChanged = true;
		ImGui::InputFloat("Background Y", &m_GameCoinBackgroundPos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			gameCoinPosChanged = true;

		// 文字偏移設定
		ImGui::Text("Text Offset (relative to background):");
		ImGui::InputFloat("Text Offset X", &m_GameCoinTextOffset.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			gameCoinOffsetChanged = true;
		ImGui::InputFloat("Text Offset Y", &m_GameCoinTextOffset.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			gameCoinOffsetChanged = true;

		// 即時更新位置
		if (gameCoinPosChanged || gameCoinOffsetChanged)
		{
			if (m_GameCoinBackground)
				m_GameCoinBackground->m_Transform.translation = m_GameCoinBackgroundPos;
			if (m_GameCoinText)
				m_GameCoinText->m_Transform.translation = m_GameCoinBackgroundPos + m_GameCoinTextOffset;
			gameCoinPosChanged = false;
			gameCoinOffsetChanged = false;
		}

		// 顯示當前遊戲幣數量
		int currentGameCoins = 0;
		if (const auto scene = SceneManager::GetInstance().GetCurrentScene().lock())
		{
			if (auto saveData = scene->GetSaveData())
			{
				currentGameCoins = saveData->gameData.gameMoney;
			}
		}
		ImGui::Text("Current Game Coins: %d", currentGameCoins);

		// 測試按鈕：增加/減少遊戲幣
		if (ImGui::Button("Add 100 Game Coins"))
		{
			if (const auto scene = SceneManager::GetInstance().GetCurrentScene().lock())
			{
				if (auto saveData = scene->GetSaveData())
				{
					saveData->gameData.gameMoney += 100;
					scene->Upload();
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Spend 50 Game Coins"))
		{
			if (const auto scene = SceneManager::GetInstance().GetCurrentScene().lock())
			{
				if (auto saveData = scene->GetSaveData())
				{
					saveData->gameData.gameMoney = std::max(0, saveData->gameData.gameMoney - 50);
					scene->Upload();
				}
			}
		}
	}

	// === 暫停按鈕位置設定 ===
	if (ImGui::CollapsingHeader("Pause Button Settings"))
	{
		static bool pausePosChanged = false;

		ImGui::Text("Pause Button Visible Position:");
		ImGui::InputFloat("Pause Button X", &m_PauseButtonVisiblePos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			pausePosChanged = true;
		ImGui::InputFloat("Pause Button Y", &m_PauseButtonVisiblePos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			pausePosChanged = true;

		if (pausePosChanged && m_PauseButton && !m_IsAnimating)
		{
			m_PauseButton->m_Transform.translation = m_PauseButtonVisiblePos;
			pausePosChanged = false;
		}
	}

	// === 面板控制 ===
	if (ImGui::CollapsingHeader("Panel Control"))
	{
		ImGui::Text("Panel Visible: %s", IsVisible() ? "Yes" : "No");
		ImGui::Text("Is Animating: %s", m_IsAnimating ? "Yes" : "No");

		if (ImGui::Button("Show Panel"))
		{
			Show();
		}
		ImGui::SameLine();
		if (ImGui::Button("Hide Panel"))
		{
			Hide();
		}
	}

	ImGui::End();
}

float GameHUDPanel::EaseOutQuad(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }
