//
// Created by QuzzS on 2025/5/1.
//

#ifndef UIPANEL_HPP
#define UIPANEL_HPP

#include <memory>
#include <string>
#include <vector>


class nGameObject;

class UIPanel
{
public:
	virtual ~UIPanel() = default;

	virtual void Start() = 0;
	virtual void Update() = 0;

	virtual void Show();
	virtual void Hide();

	virtual bool IsVisible() const { return m_IsVisible; }

	// 設置和獲取面板名稱（用於輸入阻擋檢查）
	void SetPanelName(const std::string &name) { m_PanelName = name; }
	const std::string &GetPanelName() const { return m_PanelName; }

protected:
	bool m_IsVisible = false;
	std::vector<std::shared_ptr<nGameObject>> m_GameObjects;
	std::string m_PanelName; // 面板名稱

	// 檢查輸入是否應該被阻擋的輔助方法
	bool ShouldBlockInput() const;
};

#endif // UIPANEL_HPP
