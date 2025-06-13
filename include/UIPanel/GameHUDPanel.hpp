#ifndef GAMEhudpanel_HPP
#define GAMEhudpanel_HPP

#include <memory>
#include "UIPanel.hpp"
#include "glm/glm.hpp"

class UIButton;
class PlayerStatusPanel;
class MinimapPanel;
class HealthComponent;
class walletComponent;
class DungeonMap;
class nGameObject;

class GameHUDPanel : public UIPanel
{
public:
	explicit GameHUDPanel(const std::shared_ptr<HealthComponent> &healthComp,
						  const std::shared_ptr<walletComponent> &walletComp,
						  const std::shared_ptr<DungeonMap> &dungeonMap = nullptr);
	~GameHUDPanel() override = default;
	void Start() override;
	void Update() override;

	// Override Show/Hide for animation
	void Show() override;
	void Hide() override;

protected:
	// 子面板
	std::shared_ptr<PlayerStatusPanel> m_PlayerStatusPanel;
	std::shared_ptr<MinimapPanel> m_MinimapPanel;

	// HUD 元素
	std::shared_ptr<UIButton> m_PauseButton;

	// 金幣顯示元素
	std::shared_ptr<nGameObject> m_CoinBackground;
	std::shared_ptr<nGameObject> m_CoinText;

	// 遊戲幣顯示元素
	std::shared_ptr<nGameObject> m_GameCoinBackground;
	std::shared_ptr<nGameObject> m_GameCoinText;

	// 動畫相關
	bool m_IsAnimating = false;
	bool m_IsShowingAnimation = false;
	float m_AnimationTimer = 0.0f;
	float m_AnimationDuration = 0.3f;

	// 位置設定
	glm::vec2 m_PauseButtonVisiblePos = glm::vec2(580.0f, 310.0f);
	glm::vec2 m_PauseButtonHiddenPos = glm::vec2(700.0f, 310.0f); // 滑動到右側隱藏

	// 金幣顯示位置設定
	glm::vec2 m_CoinBackgroundPos = glm::vec2(490.0f, 310.0f); // 左上角位置
	glm::vec2 m_CoinTextOffset = glm::vec2(25.0f, 0.0f); // 相對於背景的偏移

	// 遊戲幣顯示位置設定（動態計算）
	glm::vec2 m_GameCoinTextOffset = glm::vec2(25.0f, 0.0f); // 相對於背景的偏移

	// 遊戲幣背景位置（用於調試UI）
	glm::vec2 m_GameCoinBackgroundPos = glm::vec2(400.0f, 310.0f);

private:
	void InitializePauseButton();
	void InitializeCoinDisplay();
	void InitializeGameCoinDisplay(); // 新增：初始化遊戲幣顯示
	void UpdateCoinDisplay();
	void UpdateGameCoinDisplay(); // 新增：更新遊戲幣顯示
	void StartShowAnimation();
	void StartHideAnimation();
	void UpdateAnimation();
	void UpdateElementsPosition(float progress);
	void DrawDebugUI(); // 調試界面
	float EaseOutQuad(float t);

	std::shared_ptr<HealthComponent> m_PlayerHealthComponent;
	std::shared_ptr<walletComponent> m_PlayerWalletComponent;
	std::shared_ptr<DungeonMap> m_DungeonMap;

	// 金幣數量快取，用於檢測變化
	int m_LastCoinAmount = -1;

	// 遊戲幣數量快取，用於檢測變化
	int m_LastGameCoinAmount = -1;

	// Debug UI 控制
	bool m_ShowDebugUI = false;
};

#endif // GAMERUDPANEL_HPP
