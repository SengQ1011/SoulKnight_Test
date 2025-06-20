//
// Created by QuzzS on 2025/5/21.
//

#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP

#include "Override/nGameObject.hpp"

class UIButton : public nGameObject
{
public:
	explicit UIButton(const std::function<void()> &callback, const bool isHoldable) : m_IsHoldable(isHoldable)
	{
		SetFunction(callback);
	}
	~UIButton() override = default;
	void Update() override;
	void OnEventReceived(const EventInfo &eventInfo) override;

	void SetFunction(const std::function<void()> &function) { onClickCallback = function; };
	void SetOnHoverCallback(const std::function<void(bool)> &callback) { onHoverCallback = callback; };
	void DrawDebugUI();

protected:
	bool IsMouseInside() const;
	bool m_IsClicking = false;
	bool m_IsHoldable = false; // 確認button是否可以按著
	bool m_WasMouseInside = false; // 追蹤上一幀鼠標是否在按鈕內
	std::function<void()> onClickCallback;
	std::function<void(bool)> onHoverCallback; // 參數為是否懸停

private:
	// debug的變數
	bool m_DebugInitialized = false;
	glm::vec2 m_DebugSize;
	glm::vec2 m_DebugPos;
	bool m_PosChangedByInput = false;
	bool m_SizeChangedByInput = false;
};

#endif // UIBUTTON_HPP
