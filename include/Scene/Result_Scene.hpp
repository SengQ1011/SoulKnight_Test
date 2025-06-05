//
// Created by QuzzS on 2025/3/4.
//

#ifndef RESULT_SCENE_HPP
#define RESULT_SCENE_HPP

#include <memory>
#include <vector>
#include "Override/nGameObject.hpp"
#include "SaveManager.hpp"
#include "Scene/Scene.hpp"
#include "UIPanel/UIButton.hpp"


class ResultScene : public Scene
{
public:
	ResultScene() : Scene(SceneType::Result) {}
	~ResultScene() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

	// Debug功能
	void DrawDebugUI();

protected:
	// 初始化方法
	void InitBackground();
	void InitGameEndTitle();
	void InitProgressBar();
	void InitStartIcon();
	void InitEndIcon();
	void InitLevelProgress();
	void InitResultTemplate(int totalSecond = 120, int killCount = 15, int dungeonMoney = 1699, int gameMoney = 16);
	void InitContinueButton();
	void InitMagicStoneIcon();
	void InitResultIcons();
	void InitResultBackground();
	void InitPlayerIcon();
	void InitButtonText();

	// 玩家圖標移動相關方法
	void UpdatePlayerIconMovement();
	void StartPlayerMovement();
	void SwitchToReturnLobbyMode();

	// UI元素
	std::shared_ptr<nGameObject> m_ResultGroupBackground = std::make_shared<nGameObject>("Background");
	std::shared_ptr<nGameObject> m_GameEndTitle = std::make_shared<nGameObject>("GameEndTitle");
	std::shared_ptr<nGameObject> m_ProgressBar = std::make_shared<nGameObject>("ProgressBar");
	std::shared_ptr<nGameObject> m_StartIcon = std::make_shared<nGameObject>("StartIcon");
	std::shared_ptr<nGameObject> m_EndIcon = std::make_shared<nGameObject>("EndIcon");
	std::shared_ptr<nGameObject> m_PlayerIcon = std::make_shared<nGameObject>("PlayerIcon");
	std::shared_ptr<nGameObject> m_LevelProgress = std::make_shared<nGameObject>("LevelProgress");

	// 結算樣板元素
	std::shared_ptr<nGameObject> m_ClearTimeText = std::make_shared<nGameObject>("ClearTimeText");
	std::shared_ptr<nGameObject> m_GoldText = std::make_shared<nGameObject>("GoldText");
	std::shared_ptr<nGameObject> m_GameCoinText = std::make_shared<nGameObject>("GameCoinText");
	std::shared_ptr<nGameObject> m_KillCountText = std::make_shared<nGameObject>("KillCountText");

	// 結算圖標元素
	std::shared_ptr<nGameObject> m_ClearTimeIcon = std::make_shared<nGameObject>("ClearTimeIcon");
	std::shared_ptr<nGameObject> m_GoldIcon = std::make_shared<nGameObject>("GoldIcon");
	std::shared_ptr<nGameObject> m_GameCoinIcon = std::make_shared<nGameObject>("GameCoinIcon");
	std::shared_ptr<nGameObject> m_KillCountIcon = std::make_shared<nGameObject>("KillCountIcon");

	// 背景
	std::shared_ptr<nGameObject> m_Background = std::make_shared<nGameObject>("Background");

	// 按鈕
	std::shared_ptr<UIButton> m_ContinueButton;
	std::shared_ptr<nGameObject> m_ButtonText = std::make_shared<nGameObject>("ButtonText");
	;

	// 魔法石圖標
	std::shared_ptr<nGameObject> m_MagicStoneIcon = std::make_shared<nGameObject>("MagicStoneIcon");

private:
	// 相對於 m_ResultGroupBackground 的偏移量常數
	static constexpr glm::vec2 CLEAR_TIME_TEXT_OFFSET = {-70.0f, 50.0f};
	static constexpr glm::vec2 CLEAR_TIME_ICON_OFFSET = {-177.0f, 50.0f};
	static constexpr glm::vec2 GOLD_TEXT_OFFSET = {150.0f, 50.0f};
	static constexpr glm::vec2 GOLD_ICON_OFFSET = {49.0f, 50.0f};
	static constexpr glm::vec2 GAME_COIN_TEXT_OFFSET = {150.0f, -50.0f};
	static constexpr glm::vec2 GAME_COIN_ICON_OFFSET = {49.0f, -50.0f};
	static constexpr glm::vec2 KILL_COUNT_TEXT_OFFSET = {-70.0f, -50.0f};
	static constexpr glm::vec2 KILL_COUNT_ICON_OFFSET = {-177.0f, -50.0f};

	// Debug相關變數
	bool m_ShowDebugUI = false;

	// 關卡進度相關變數
	PlayerData m_PlayerData;
	GameProgress m_GameProgress;

	// 玩家圖標移動相關變數
	static constexpr float PLAYER_MOVE_SPEED = 200.0f; // 每秒移動像素
	static constexpr float STAGE_SEGMENT_LENGTH = 156.0f; // 每段的長度 (780/5)
	bool m_IsPlayerMoving = false;
	bool m_HasReachedEnd = false;
	glm::vec2 m_PlayerTargetPosition;
	glm::vec2 m_PlayerStartPosition;
};

#endif // RESULT_SCENE_HPP
