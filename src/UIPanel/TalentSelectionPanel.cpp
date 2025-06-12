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

TalentSelectionPanel::TalentSelectionPanel()
{
	// 從天賦數據庫獲取所有可用天賦
	m_AvailableTalents = CreateTalentList();

	// 初始化時先嘗試從 SceneManager 獲取數據
	LoadPlayerTalentData();
}

void TalentSelectionPanel::SetSharedSaveData(std::shared_ptr<SaveData> saveData)
{
	m_SharedSaveData = saveData;
	m_UseSharedData = (saveData != nullptr);

	if (m_UseSharedData)
	{
		// 重新加載玩家天賦數據
		LoadPlayerTalentData();
	}
}

void TalentSelectionPanel::LoadPlayerTalentData()
{
	if (m_UseSharedData && m_SharedSaveData)
	{
		// 使用共享數據
		m_PlayerTalentIDs = m_SharedSaveData->gameProgress.playerData.talentID;
	}
	else
	{
		// 回退到從 SceneManager 獲取數據
		auto &sceneManager = SceneManager::GetInstance();
		auto saveData = sceneManager.DownloadGameProgress();
		if (saveData)
		{
			m_PlayerTalentIDs = saveData->gameProgress.playerData.talentID;
		}
		else
		{
			LOG_ERROR("Failed to get save data from SceneManager");
			m_PlayerTalentIDs.clear();
		}
	}
}

void TalentSelectionPanel::SaveTalentToSharedData(int talentId)
{
	if (m_SharedSaveData)
	{
		m_SharedSaveData->gameProgress.playerData.talentID.push_back(talentId);
		m_PlayerTalentIDs.push_back(talentId); // 同步本地緩存

		// 增加天賦選擇計數
		m_SharedSaveData->gameProgress.talentSelectionCount++;

		// 立即保存到磁盤
		SaveManager::GetInstance().SaveGame(m_SharedSaveData);
	}
	else
	{
		LOG_ERROR("Shared save data is null, cannot save talent");
	}
}

void TalentSelectionPanel::SaveTalentToSceneManager(int talentId)
{
	// 使用新的安全方法
	auto &sceneManager = SceneManager::GetInstance();
	sceneManager.AddTalentToPlayer(talentId);
	sceneManager.IncrementTalentSelectionCount(); // 增加天賦選擇計數
	m_PlayerTalentIDs.push_back(talentId); // 同步本地緩存
}

void TalentSelectionPanel::Start()
{
	// 設置面板名稱用於輸入阻擋檢查
	SetPanelName("talent_selection");

	// 創建遮罩層 - 使用半透明黑色
	m_OverLay = std::make_shared<nGameObject>("TalentOverlay");
	auto overlayImage = ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/overlay_black.png");
	if (!overlayImage)
	{
		LOG_ERROR("Failed to load overlay image");
	}
	m_OverLay->SetDrawable(overlayImage);
	m_OverLay->SetZIndex(90.0f);
	// 設置遮罩層大小為屏幕大小
	m_OverLay->m_Transform.scale =
		glm::vec2(static_cast<float>(PTSD_Config::WINDOW_WIDTH), static_cast<float>(PTSD_Config::WINDOW_HEIGHT));
	m_OverLay->SetVisible(true);
	m_GameObjects.push_back(m_OverLay);

	// 創建面板背景 - 使用半透明背景
	m_PanelBackground = std::make_shared<nGameObject>("TalentBackground");
	auto bgImage = ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/black.png");
	if (!bgImage)
	{
		LOG_ERROR("Failed to load background image");
	}
	m_PanelBackground->SetDrawable(bgImage);
	m_PanelBackground->SetZIndex(m_OverLay->GetZIndex() + 0.1f);
	// 設置背景位置在屏幕中央
	m_PanelBackground->m_Transform.scale =
		glm::vec2(static_cast<float>(PTSD_Config::WINDOW_WIDTH), static_cast<float>(PTSD_Config::WINDOW_HEIGHT)) /
		m_PanelBackground->GetImageSize();
	m_PanelBackground->SetVisible(true);
	m_GameObjects.push_back(m_PanelBackground);

	// 創建標題文字
	m_TitleText = std::make_shared<nGameObject>("TalentTitle");
	const auto titleText = ImagePoolManager::GetInstance().GetText(RESOURCE_DIR "/Font/jf-openhuninn-2.1.ttf", 48.0f,
																   "選擇天賦", Util::Color(255, 255, 255), false);
	if (!titleText)
	{
		LOG_ERROR("Failed to create title text");
	}
	m_TitleText->SetDrawable(titleText);
	m_TitleText->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	// 將標題放在背景上方
	m_TitleText->m_Transform.translation.y =
		(static_cast<float>(PTSD_Config::WINDOW_HEIGHT) - m_TitleText->GetScaledSize().y) / 2 - 75.0f;
	m_TitleText->m_Transform.translation.x = 8.0f;
	m_TitleText->SetVisible(true);
	m_GameObjects.push_back(m_TitleText);

	// 初始化天賦按鈕
	bool hasAvailableTalents = InitializeTalentButtons();

	// 添加到場景
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		if (const auto renderer = scene->GetRoot().lock())
		{
			// 確保所有元素都被添加到場景中
			for (const auto &gameObject : m_GameObjects)
			{
				if (gameObject)
				{
					renderer->AddChild(gameObject);
					gameObject->SetVisible(true);
				}
			}
		}
		else
		{
			LOG_ERROR("Failed to get renderer");
		}
	}
	else
	{
		LOG_ERROR("Failed to get current scene");
	}

	// 根據是否有可用天賦來決定顯示或隱藏面板
	if (hasAvailableTalents)
	{
		Show();
	}
	else
	{
		Hide();
	}
}

bool TalentSelectionPanel::InitializeTalentButtons()
{
	m_TalentButtons.clear();
	m_TalentButtons.resize(TALENT_OPTIONS);

	// 檢查玩家當前的關卡進度和天賦選擇次數
	int currentProgress = 0;
	int talentSelectionCount = 0; // 改為使用天賦選擇次數

	if (m_UseSharedData && m_SharedSaveData)
	{
		const auto &gameProgress = m_SharedSaveData->gameProgress;
		// 確保關卡數據的有效性
		int chapter = std::max(1, gameProgress.currentChapter);
		int stage = std::max(1, gameProgress.currentStage);
		currentProgress = (chapter - 1) * 5 + stage;
		talentSelectionCount = gameProgress.talentSelectionCount;
	}
	else
	{
		// 從 SceneManager 獲取進度
		auto &sceneManager = SceneManager::GetInstance();
		auto saveData = sceneManager.DownloadGameProgress();
		if (saveData)
		{
			const auto &gameProgress = saveData->gameProgress;
			// 確保關卡數據的有效性
			int chapter = std::max(1, gameProgress.currentChapter);
			int stage = std::max(1, gameProgress.currentStage);
			currentProgress = (chapter - 1) * 5 + stage;
			talentSelectionCount = gameProgress.talentSelectionCount;
		}
		else
		{
			LOG_ERROR("Failed to get save data, defaulting to progress 1");
			currentProgress = 1; // 默認為第一關
			talentSelectionCount = 0; // 默認為0次選擇
		}
	}

	// 如果天賦選擇次數已經達到或超過當前關卡進度，則不顯示天賦選擇
	if (talentSelectionCount >= currentProgress)
	{
		return false;
	}

	// 計算按鈕位置
	const float buttonSpacing = 300.0f; // 按鈕間距
	const float totalWidth = (TALENT_OPTIONS - 1) * buttonSpacing;
	const float startX = -totalWidth / 2.0f;
	const float buttonY = 0.0f; // 按鈕垂直位置

	// 過濾出玩家未擁有的天賦
	std::vector<Talent> availableTalents;
	for (const auto &talent : m_AvailableTalents)
	{
		if (std::find(m_PlayerTalentIDs.begin(), m_PlayerTalentIDs.end(), talent.GetId()) == m_PlayerTalentIDs.end())
		{
			availableTalents.push_back(talent);
		}
	}

	// 如果沒有可用天賦，返回 false
	if (availableTalents.empty())
	{
		return false;
	}

	// 隨機選擇天賦
	std::vector<Talent> selectedTalents;
	if (availableTalents.size() <= TALENT_OPTIONS)
	{
		selectedTalents = availableTalents;
	}
	else
	{
		// 使用 Fisher-Yates 洗牌算法
		for (int i = 0; i < TALENT_OPTIONS; ++i)
		{
			int remaining = static_cast<int>(availableTalents.size()) - i;
			int randomIndex = RandomUtil::RandomIntInRange(0, remaining - 1);
			selectedTalents.push_back(availableTalents[randomIndex]);
			std::swap(availableTalents[randomIndex], availableTalents[remaining - 1]);
		}
	}

	// 創建天賦按鈕
	for (int i = 0; i < TALENT_OPTIONS; ++i)
	{
		// 創建按鈕背景
		auto buttonBg = std::make_shared<nGameObject>("TalentButtonBg_" + std::to_string(i));
		auto bgImage = ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_talentIcon/ui_buff_00.png");
		if (!bgImage)
		{
			LOG_ERROR("Failed to load button background image");
		}
		buttonBg->SetDrawable(bgImage);
		buttonBg->m_Transform.translation = glm::vec2(startX + i * buttonSpacing, buttonY);
		buttonBg->m_Transform.scale = glm::vec2(3.0f);
		buttonBg->SetVisible(true);
		buttonBg->SetZIndex(m_PanelBackground->GetZIndex() + 0.2f);
		m_GameObjects.push_back(buttonBg);

		// 創建按鈕
		auto button = std::make_shared<UIButton>(nullptr, false);
		button->SetZIndex(buttonBg->GetZIndex() + 0.1f);
		button->m_Transform.translation = buttonBg->m_Transform.translation;
		button->m_Transform.scale = buttonBg->m_Transform.scale;

		if (i < static_cast<int>(selectedTalents.size()))
		{
			const auto &talent = selectedTalents[i];
			const std::string iconPath = RESOURCE_DIR + talent.GetIconPath();
			auto image = ImagePoolManager::GetInstance().GetImage(iconPath);
			if (!image)
			{
				LOG_ERROR("Failed to load talent icon: {}", iconPath);
				// 使用默認圖片作為後備
				image = ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_talentIcon/ui_buff_00.png");
			}
			button->SetDrawable(image);

			// 創建天賦名稱文字
			auto nameText = std::make_shared<nGameObject>("TalentName_" + std::to_string(i));
			auto textDrawable = ImagePoolManager::GetInstance().GetText(RESOURCE_DIR "/Font/jf-openhuninn-2.1.ttf",
																		32.0f, // 增大字體大小
																		talent.GetName(), Util::Color(255, 255, 255));
			if (textDrawable)
			{
				nameText->SetDrawable(textDrawable);
				nameText->SetZIndex(button->GetZIndex() + 0.2f); // 確保文字在最上層
				// 將文字放在按鈕下方，並調整位置
				const float offsetY = (nameText->GetScaledSize().y + button->GetScaledSize().y) / 2.0f + 25.0f;
				nameText->m_Transform.translation = button->m_Transform.translation + glm::vec2(8.0f, offsetY);
				nameText->SetVisible(true);
				m_GameObjects.push_back(nameText);
			}

			// 設置按鈕回調
			button->SetFunction(
				[this, talent, i]()
				{
					// 使用新的共享數據保存方法
					if (m_UseSharedData)
					{
						SaveTalentToSharedData(talent.GetId());
					}
					else
					{
						SaveTalentToSceneManager(talent.GetId());
					}

					AudioManager::GetInstance().PlaySFX("click");

					// 開始天賦選擇動畫
					StartTalentSelectionAnimation(i);
				});
		}
		else
		{
			auto defaultImage =
				ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_talentIcon/ui_buff_00.png");
			if (!defaultImage)
			{
				LOG_ERROR("Failed to load default icon");
			}
			button->SetDrawable(defaultImage);
		}

		button->SetVisible(true);
		m_TalentButtons[i] = button;
		m_GameObjects.push_back(button);
	}

	// 創建天賦黑色遮擋方塊
	CreateTalentBlackBoxes();

	// 創建天賦懸停邊框
	CreateTalentHoverBorders();

	return true;
}

void TalentSelectionPanel::CreateTalentBlackBoxes()
{
	m_TalentBlackBoxes.clear();
	m_TalentBlackBoxes.resize(TALENT_OPTIONS);

	for (int i = 0; i < TALENT_OPTIONS; ++i)
	{
		if (m_TalentButtons[i])
		{
			// 創建黑色遮擋方塊
			auto blackBox = std::make_shared<nGameObject>("TalentBlackBox_" + std::to_string(i));
			auto blackImage = ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/black.png");
			if (!blackImage)
			{
				LOG_ERROR("Failed to load black box image");
			}
			blackBox->SetDrawable(blackImage);

			// 設置位置和Z-Index
			blackBox->SetPivot(glm::vec2(0.0f, blackBox->GetImageSize().y / 2.0f));
			blackBox->m_Transform.translation = m_TalentButtons[i]->m_Transform.translation +
				glm::vec2(0.0f, m_TalentButtons[i]->GetScaledSize().y / 2.0f);
			blackBox->SetZIndex(m_TalentButtons[i]->GetZIndex() + 0.1f);

			// 計算縮放比例：x軸貼合按鈕大小，y軸幾乎不可見
			glm::vec2 buttonSize = m_TalentButtons[i]->GetScaledSize();
			glm::vec2 blackBoxImageSize = blackBox->GetImageSize();
			glm::vec2 targetScale = buttonSize / blackBoxImageSize;

			blackBox->m_Transform.scale = glm::vec2(targetScale.x, 0.001f);
			blackBox->SetVisible(false); // 初始狀態不可見

			m_TalentBlackBoxes[i] = blackBox;
			m_GameObjects.push_back(blackBox);
		}
	}
}

void TalentSelectionPanel::Update()
{
	// 更新所有UI元素（但不強制設置懸停邊框的可見性）
	for (const auto &gameObject : m_GameObjects)
	{
		if (gameObject) gameObject->Update();
	}

	// 更新天賦選擇動畫
	UpdateTalentSelectionAnimation();
}

void TalentSelectionPanel::Show()
{
	// 調用基類的Show方法設置基本可見性
	UIPanel::Show();

	// 但是需要特別處理懸停邊框，它們應該保持隱藏狀態
	for (const auto &hoverBorder : m_TalentHoverBorders)
	{
		if (hoverBorder)
		{
			hoverBorder->SetVisible(false);
		}
	}

	// 重置天賦選擇動畫狀態
	m_IsTalentSelected = false;
	m_SelectedTalentIndex = -1;
	m_BlackBoxAnimationTimer.Stop();
	m_BlackBoxAnimationTimer.Reset();

	// 確保黑色遮擋方塊也是隱藏的
	for (const auto &blackBox : m_TalentBlackBoxes)
	{
		if (blackBox)
		{
			blackBox->SetVisible(false);
		}
	}
}

void TalentSelectionPanel::CreateTalentHoverBorders()
{
	m_TalentHoverBorders.clear();
	m_TalentHoverBorders.resize(TALENT_OPTIONS);

	for (int i = 0; i < TALENT_OPTIONS; ++i)
	{
		if (m_TalentButtons[i])
		{
			// 創建懸停邊框
			auto hoverBorder = std::make_shared<nGameObject>("TalentHoverBorder_" + std::to_string(i));
			auto borderImage = ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/Sprite/ui-all/ui_blue_frame.png");
			if (!borderImage)
			{
				LOG_ERROR("Failed to load hover border image");
			}
			hoverBorder->SetDrawable(borderImage);

			// 設置位置和縮放，讓邊框稍微大於按鈕
			hoverBorder->m_Transform.translation = m_TalentButtons[i]->m_Transform.translation;
			glm::vec2 buttonSize = m_TalentButtons[i]->GetScaledSize();
			glm::vec2 borderImageSize = hoverBorder->GetImageSize();
			glm::vec2 borderScale = (buttonSize * 1.1f) / borderImageSize; // 比按鈕大10%
			hoverBorder->m_Transform.scale = borderScale;
			hoverBorder->SetZIndex(m_TalentButtons[i]->GetZIndex() - 0.1f); // 在按鈕下方
			hoverBorder->SetVisible(false); // 初始狀態不可見

			m_TalentHoverBorders[i] = hoverBorder;
			m_GameObjects.push_back(hoverBorder);

			// 設置按鈕的懸停回調
			m_TalentButtons[i]->SetOnHoverCallback(
				[this, i](bool isHovering)
				{
					if (m_TalentHoverBorders[i])
					{
						m_TalentHoverBorders[i]->SetVisible(isHovering);
					}
				});
		}
	}
}

void TalentSelectionPanel::StartTalentSelectionAnimation(int selectedIndex)
{
	m_IsTalentSelected = true;
	m_SelectedTalentIndex = selectedIndex;

	// 設定並啟動動畫計時器
	m_BlackBoxAnimationTimer.SetDuration(m_BlackBoxAnimationDuration);
	m_BlackBoxAnimationTimer.Start();

	// 設定動畫完成後的回調
	m_BlackBoxAnimationTimer.SetOnFinished(
		[this]()
		{
			// 動畫完成後延遲0.5秒隱藏面板
			// 這裡可以再用一個Timer，但為了簡單起見先保持原有邏輯
			Hide();
		});

	// 顯示除了選中天賦外的所有黑色方塊
	for (int i = 0; i < m_TalentBlackBoxes.size(); ++i)
	{
		if (i != selectedIndex && m_TalentBlackBoxes[i])
		{
			m_TalentBlackBoxes[i]->SetVisible(true);
		}
	}
}

void TalentSelectionPanel::UpdateTalentSelectionAnimation()
{
	if (!m_IsTalentSelected || !m_BlackBoxAnimationTimer.IsRunning())
		return;

	// 更新計時器
	m_BlackBoxAnimationTimer.Update();

	// 使用緩動函數獲取動畫進度
	float progress = m_BlackBoxAnimationTimer.GetEasedProgress(Util::Timer::EaseOutQuad);

	// 更新所有非選中天賦的黑色方塊縮放
	for (int i = 0; i < m_TalentBlackBoxes.size(); ++i)
	{
		if (i != m_SelectedTalentIndex && m_TalentBlackBoxes[i])
		{
			auto &blackBox = m_TalentBlackBoxes[i];
			float targetSize = m_TalentButtons[i]->GetScaledSize().y / blackBox->GetImageSize().y;
			glm::vec2 currentScale = blackBox->m_Transform.scale;
			currentScale.y = 0.001f + progress * (targetSize - 0.001f);
			blackBox->m_Transform.scale = currentScale;
		}
	}
}
