//
// Created by QuzzS on 2025/5/1.
//

#ifndef UIPANEL_HPP
#define UIPANEL_HPP

#include <memory>
#include <vector>
class nGameObject;

class UIPanel {
public:
	virtual ~UIPanel() = default;

	virtual void Start() = 0;
	virtual void Update() = 0;

	virtual void Show();
	virtual void Hide();
	virtual bool IsVisible() const {return m_IsVisible;}
protected:
	bool m_IsVisible = false;
	std::vector<std::shared_ptr<nGameObject>> m_GameObjects;
};

#endif //UIPANEL_HPP
