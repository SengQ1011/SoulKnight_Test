//
// Created by QuzzS on 2025/5/1.
//

#ifndef SETTINGPANEL_HPP
#define SETTINGPANEL_HPP

#include <memory>
#include <vector>


#include "UIPanel.hpp"
class nGameObject;

class SettingPanel : public UIPanel {
public:
	void Start() override;
	void Update() override;
	void DrawDebugUI();

};

#endif //SETTINGPANEL_HPP
