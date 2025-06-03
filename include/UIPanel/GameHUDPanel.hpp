#ifndef GAMEhudpanel_HPP
#define GAMEhudpanel_HPP

#include <memory>
#include "UIPanel.hpp"
#include "glm/glm.hpp"

class UIButton;
class PlayerStatusPanel;
class HealthComponent;
class WalletComponent;
class nGameObject;

class GameHUDPanel : public UIPanel
{
public:
	explicit GameHUDPanel(const std::shared_ptr<HealthComponent> &healthComp,
						  const std::shared_ptr<WalletComponent> &walletComp);
	~GameHUDPanel() override = default;
	void Start() override;
	void Update() override;

	// Override Show/Hide for animation
	void Show() override;
	void Hide() override;

protected:
	// 子面板
	std::shared_ptr<PlayerStatusPanel> m_PlayerStatusPanel;

	// HUD 元素
	std::shared_ptr<UIButton> m_PauseButton;

	// 金幣顯示元素
	std::shared_ptr<nGameObject> m_CoinBackground;
	std::shared_ptr<nGameObject> m_CoinText;

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

private:
	void InitializePauseButton();
	void InitializeCoinDisplay();
	void UpdateCoinDisplay();
	void StartShowAnimation();
	void StartHideAnimation();
	void UpdateAnimation();
	void UpdateElementsPosition(float progress);
	void DrawDebugUI(); // 調試界面
	float EaseOutQuad(float t);

	std::shared_ptr<HealthComponent> m_PlayerHealthComponent;
	std::shared_ptr<WalletComponent> m_PlayerWalletComponent;

	// 金幣數量快取，用於檢測變化
	int m_LastCoinAmount = -1;
};

#endif // GAMERUDPANEL_HPP
