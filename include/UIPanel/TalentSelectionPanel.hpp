//
// Created by tjx20 on 6/12/2025.
//

#ifndef TALENTSELECTIONPANEL_HPP
#define TALENTSELECTIONPANEL_HPP

#include "UIPanel.hpp"
#include "GameMechanism/Talent.hpp"
#include <memory>
#include <vector>
#include "glm/glm.hpp"

class UIButton;
class nGameObject;

class TalentSelectionPanel : public UIPanel {
public:
	TalentSelectionPanel();
	~TalentSelectionPanel() override = default;

	void Start() override;
	void Update() override;
	void DrawDebugUI();

	void Show() override;
	void Hide() override;

protected:
	std::shared_ptr<nGameObject> m_OverLay; // Overlay background
	std::shared_ptr<nGameObject> m_PanelBackground;
	std::vector<std::shared_ptr<UIButton>> m_TalentButtons;
	std::shared_ptr<nGameObject> m_TitleText;

	bool m_IsAnimating = false;
	bool m_IsShowingAnimation = false;
	float m_AnimationTimer = 0.0f;
	float m_AnimationDuration = 0.4f;
	glm::vec2 m_VisiblePosition = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_HiddenPosition = glm::vec2(0.0f, 1000.0f);

	std::vector<Talent> m_AvailableTalents;
	std::vector<int> m_PlayerTalentIDs;  // 玩家已擁有的天賦ID
	static const int TALENT_OPTIONS = 3; // 會顯示可選擇天賦的數量

private:
	void InitializeTalentButtons();
	void UpdateTalentButtons();
	void StartShowAnimation();
	void StartHideAnimation();
	void UpdateAnimation();
	void UpdatePanelPosition(float progress);
	void UpdateAllElementsPosition(const glm::vec2& panelPosition);
	float EaseOutQuad(float t);
};


#endif //TALENTSELECTIONPANEL_HPP
