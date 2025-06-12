//
// Created by tjx20 on 6/12/2025.
//

#ifndef TALENTSELECTIONPANEL_HPP
#define TALENTSELECTIONPANEL_HPP

#include <memory>
#include <vector>
#include "GameMechanism/Talent.hpp"
#include "UIPanel.hpp"
#include "Util/Timer.hpp"
#include "glm/glm.hpp"


class UIButton;
class nGameObject;
struct SaveData;

class TalentSelectionPanel : public UIPanel
{
public:
	TalentSelectionPanel();
	~TalentSelectionPanel() override = default;

	void Start() override;
	void Update() override;

	// 共享數據方法
	void SetSharedSaveData(std::shared_ptr<SaveData> saveData);
	void LoadPlayerTalentData();

	void Show() override;

protected:
	std::shared_ptr<nGameObject> m_OverLay; // Overlay background
	std::shared_ptr<nGameObject> m_PanelBackground;
	std::vector<std::shared_ptr<UIButton>> m_TalentButtons;
	std::vector<std::shared_ptr<nGameObject>> m_TalentBlackBoxes; // 天賦遮擋黑色方塊
	std::vector<std::shared_ptr<nGameObject>> m_TalentHoverBorders; // 天賦懸停邊框
	std::shared_ptr<nGameObject> m_TitleText;

	bool m_IsAnimating = false;
	bool m_IsShowingAnimation = false;
	float m_AnimationTimer = 0.0f;
	float m_AnimationDuration = 0.4f;
	glm::vec2 m_VisiblePosition = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_HiddenPosition = glm::vec2(0.0f, 1000.0f);

	// 天賦選擇動畫相關
	bool m_IsTalentSelected = false; // 是否已選擇天賦
	int m_SelectedTalentIndex = -1; // 選中的天賦索引
	Util::Timer m_BlackBoxAnimationTimer; // 黑色方塊動畫計時器
	float m_BlackBoxAnimationDuration = 1.0f; // 黑色方塊動畫持續時間

	std::vector<Talent> m_AvailableTalents;
	std::vector<int> m_PlayerTalentIDs; // 玩家已擁有的天賦ID
	static const int TALENT_OPTIONS = 3; // 會顯示可選擇天賦的數量

	// 共享存檔數據引用
	std::shared_ptr<SaveData> m_SharedSaveData;
	bool m_UseSharedData = true;

private:
	bool InitializeTalentButtons();

	// 天賦選擇動畫相關方法
	void StartTalentSelectionAnimation(int selectedIndex);
	void UpdateTalentSelectionAnimation();
	void CreateTalentBlackBoxes();
	void CreateTalentHoverBorders();

	// 數據操作方法
	void SaveTalentToSharedData(int talentId);
	void SaveTalentToSceneManager(int talentId);
};


#endif // TALENTSELECTIONPANEL_HPP
