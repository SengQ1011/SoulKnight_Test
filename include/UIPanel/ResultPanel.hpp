// //
// // Created by Assistant on 2025/1/2.
// //
//
// #ifndef RESULTPANEL_HPP
// #define RESULTPANEL_HPP
//
// #include <functional>
// #include <memory>
// #include "SaveManager.hpp"
// #include "UIPanel.hpp"
// #include "glm/glm.hpp"
//
//
// class UIButton;
// class nGameObject;
//
// class ResultPanel : public UIPanel
// {
// public:
// 	// 結算數據結構
// 	struct ResultData
// 	{
// 		int endTime;
// 		int killCount;
// 		int dungeonMoney;
// 		int gameMoney;
// 		int timeScore;
// 		int killScore;
// 		int totalScore;
// 	};
//
// 	// 面板狀態
// 	enum class State
// 	{
// 		Initial, // 初始狀態
// 		PlayerMoving, // 玩家移動中
// 		ShowingResults, // 顯示結果
// 		ReadyToReturn // 準備返回
// 	};
//
// 	explicit ResultPanel(const ResultData &data, const GameProgress &progress);
// 	~ResultPanel() override = default;
//
// 	void Start() override;
// 	void Update() override;
//
// 	// 狀態管理
// 	State GetCurrentState() const { return m_CurrentState; }
// 	void StartPlayerMovement();
// 	void SwitchToReturnMode();
//
// private:
// 	// UI元素統一管理
// 	struct UIElements
// 	{
// 		std::shared_ptr<nGameObject> background;
// 		std::shared_ptr<nGameObject> resultGroupBackground;
// 		std::shared_ptr<nGameObject> gameEndTitle;
// 		std::shared_ptr<nGameObject> progressBar;
// 		std::shared_ptr<nGameObject> startIcon;
// 		std::shared_ptr<nGameObject> endIcon;
// 		std::shared_ptr<nGameObject> playerIcon;
// 		std::shared_ptr<nGameObject> levelProgress;
//
// 		// 結算文字
// 		std::shared_ptr<nGameObject> clearTimeText;
// 		std::shared_ptr<nGameObject> goldText;
// 		std::shared_ptr<nGameObject> gameCoinText;
// 		std::shared_ptr<nGameObject> killCountText;
//
// 		// 結算圖標
// 		std::shared_ptr<nGameObject> clearTimeIcon;
// 		std::shared_ptr<nGameObject> goldIcon;
// 		std::shared_ptr<nGameObject> gameCoinIcon;
// 		std::shared_ptr<nGameObject> killCountIcon;
//
// 		std::shared_ptr<UIButton> continueButton;
// 		std::shared_ptr<nGameObject> buttonText;
// 		std::shared_ptr<nGameObject> magicStoneIcon;
// 	};
//
// 	// 內部動畫管理類別 - 避免過度設計
// 	class PlayerMovementManager
// 	{
// 	public:
// 		PlayerMovementManager(std::shared_ptr<nGameObject> playerIcon, std::shared_ptr<nGameObject> levelProgress,
// 							  std::shared_ptr<nGameObject> startIcon, std::shared_ptr<nGameObject> endIcon,
// 							  const GameProgress &progress);
//
// 		void StartMovement(std::function<void()> onComplete = nullptr);
// 		bool UpdateMovement(float deltaTime);
// 		void UpdateProgressText(int currentStage);
//
// 		bool IsMoving() const { return m_IsMoving; }
//
// 	private:
// 		std::shared_ptr<nGameObject> m_PlayerIcon;
// 		std::shared_ptr<nGameObject> m_LevelProgress;
// 		std::shared_ptr<nGameObject> m_StartIcon;
// 		std::shared_ptr<nGameObject> m_EndIcon;
// 		GameProgress m_GameProgress;
//
// 		bool m_IsMoving = false;
// 		glm::vec2 m_StartPosition;
// 		glm::vec2 m_TargetPosition;
// 		std::function<void()> m_OnCompleteCallback;
//
// 		static constexpr float MOVE_SPEED = 200.0f;
// 	};
//
// 	UIElements m_Elements;
// 	ResultData m_Data;
// 	GameProgress m_GameProgress;
// 	State m_CurrentState = State::Initial;
//
// 	std::unique_ptr<PlayerMovementManager> m_MovementManager;
//
// 	// 相對於 m_ResultGroupBackground 的偏移量常數
// 	static constexpr glm::vec2 CLEAR_TIME_TEXT_OFFSET = {-70.0f, 50.0f};
// 	static constexpr glm::vec2 CLEAR_TIME_ICON_OFFSET = {-177.0f, 50.0f};
// 	static constexpr glm::vec2 GOLD_TEXT_OFFSET = {150.0f, 50.0f};
// 	static constexpr glm::vec2 GOLD_ICON_OFFSET = {49.0f, 50.0f};
// 	static constexpr glm::vec2 GAME_COIN_TEXT_OFFSET = {150.0f, -50.0f};
// 	static constexpr glm::vec2 GAME_COIN_ICON_OFFSET = {49.0f, -50.0f};
// 	static constexpr glm::vec2 KILL_COUNT_TEXT_OFFSET = {-70.0f, -50.0f};
// 	static constexpr glm::vec2 KILL_COUNT_ICON_OFFSET = {-177.0f, -50.0f};
//
// 	// 初始化方法
// 	void InitializeAllElements();
// 	void InitializeBackground();
// 	void InitializeGameEndTitle();
// 	void InitializeProgressBar();
// 	void InitializeIcons();
// 	void InitializeLevelProgress();
// 	void InitializeResultTexts();
// 	void InitializeResultIcons();
// 	void InitializeButtons();
//
// 	// 狀態管理
// 	void TransitionToState(State newState);
// 	void UpdateCurrentState();
//
// 	// 更新方法
// 	void UpdateEndIconRotation();
//
// 	// 事件處理 - 使用EventManager
// 	void OnContinueButtonClicked();
// 	void OnReturnLobbyButtonClicked();
// 	void SubscribeToEvents();
// 	void UnsubscribeFromEvents();
//
// 	// EventManager監聽器
// 	void OnPlayerMovementComplete(const EventInfo &eventInfo);
// 	void OnButtonStateChange(const EventInfo &eventInfo);
//
// 	// 直接調用方法，避免事件系統問題
// 	void OnPlayerMovementComplete_Direct();
// };
//
// #endif // RESULTPANEL_HPP
