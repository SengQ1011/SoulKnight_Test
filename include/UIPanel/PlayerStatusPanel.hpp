//
// Created by QuzzS on 2025/5/29.
//

#ifndef PLAYERSTATUSPANEL_HPP
#define PLAYERSTATUSPANEL_HPP

#include <memory>

#include "UIPanel.hpp"


class HealthComponent;
class UIButton;
class UISlider;
class nGameObject;

class PlayerStatusPanel : public UIPanel {
public:
	explicit PlayerStatusPanel(const std::shared_ptr<HealthComponent>& healthComp) : m_PlayerHealthComponent(healthComp) {}
	~PlayerStatusPanel() override = default;
	void Start() override;
	void Update() override;
	void DrawDebugUI();
protected:
	std::weak_ptr<HealthComponent> m_PlayerHealthComponent;
	std::shared_ptr<nGameObject> m_PanelBackground;
	std::shared_ptr<UISlider> m_SliderPlayerHP;
	std::shared_ptr<UISlider> m_SliderPlayerArmor;
	std::shared_ptr<UISlider> m_SliderPlayerEnergy;
	std::shared_ptr<nGameObject> m_TextPlayerHP;
	std::shared_ptr<nGameObject> m_TextPlayerArmor;
	std::shared_ptr<nGameObject> m_TextPlayerEnergy;
};

#endif //PLAYERSTATUSPANEL_HPP
