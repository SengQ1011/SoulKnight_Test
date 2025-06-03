//
// Created by QuzzS on 2025/5/21.
//

#include "UIPanel/UIButton.hpp"

#include "ObserveManager/AudioManager.hpp"
#include "Structs/CollisionComponentStruct.hpp" //???Rect??????
#include "Tool/Tool.hpp"
#include "Util/Input.hpp"

void UIButton::Update()
{
	nGameObject::Update();
	if (!m_Active) return; // 不活躍不執行互動

	DrawDebugUI(); // 顯示調整用 ImGui 視窗

	// -----------------------
	// 支援可長按的按鈕邏輯
	// -----------------------
	if (m_IsHoldable)
	{
		// 滑鼠按下且在範圍內才算點擊中
		if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB) && IsMouseInside())
			m_IsClicking = true;
		else if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB) && m_IsClicking)
		{
			AudioManager::GetInstance().PlaySFX("click");
			m_IsClicking = false;
		}

		// 若未點擊則不執行 callback
		if (!m_IsClicking) return;

		if (onClickCallback)
			onClickCallback();
	}
	// -----------------------
	// 一般按鈕邏輯（點擊一下就觸發）
	// -----------------------
	else
	{
		if (!Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB) || !IsMouseInside()) return;

		if (onClickCallback)
		{
			onClickCallback();
			AudioManager::GetInstance().PlaySFX("click");
		}
	}
}

// ==========================
// 判斷滑鼠是否在按鈕範圍內
// ==========================
bool UIButton::IsMouseInside() const
{
	glm::vec2 cursor = Tool::GetMouseCoord();
	const Rect bound(m_Transform.translation, GetScaledSize());

	// 判斷是否超出邊界
	return !(cursor.x < bound.left() || cursor.x > bound.right() ||
			 cursor.y > bound.top() || cursor.y < bound.bottom());
}

// ==========================
// 接收外部事件
// ==========================
void UIButton::OnEventReceived(const EventInfo &eventInfo)
{
	switch (eventInfo.GetEventType())
	{
	case EventType::Click:
	{
		if (onClickCallback) onClickCallback();
		break;
	}
	default:
		break;
	}
}

// ==========================
// ImGui Debug 視窗（控制尺寸與位置）
// ==========================
void UIButton::DrawDebugUI()
{
	// 初始化 DebugSize/Pos（只做一次）
	if (!m_DebugInitialized && m_Drawable)
	{
		m_DebugSize = m_Transform.scale * GetImageSize();
		m_DebugPos = m_Transform.translation;
		m_DebugInitialized = true;
	}

	// 根據使用者輸入更新 Transform 狀態
	if (m_SizeChangedByInput)
	{
		m_Transform.scale = m_DebugSize / GetImageSize();
		m_SizeChangedByInput = false;
	}
	if (m_PosChangedByInput)
	{
		m_Transform.translation = m_DebugPos;
		m_PosChangedByInput = false;
	}

	// 彈出調整視窗
	const std::string name = "SetPosition_Button_" + std::to_string(m_Id);
	ImGui::Begin(name.c_str());

	// 大小調整
	ImGui::InputFloat("Setting Width", &m_DebugSize.x, 1.0f);
	if (ImGui::IsItemDeactivatedAfterEdit()) m_SizeChangedByInput = true;

	ImGui::InputFloat("Setting Height", &m_DebugSize.y, 1.0f);
	if (ImGui::IsItemDeactivatedAfterEdit()) m_SizeChangedByInput = true;

	// 位置調整
	ImGui::InputFloat("Setting x", &m_DebugPos.x, 1.0f);
	if (ImGui::IsItemDeactivatedAfterEdit()) m_PosChangedByInput = true;

	ImGui::InputFloat("Setting y", &m_DebugPos.y, 1.0f);
	if (ImGui::IsItemDeactivatedAfterEdit()) m_PosChangedByInput = true;

	// 不要每幀覆蓋 m_DebugSize/m_DebugPos，否則用戶輸入會被吃掉
	if (!m_SizeChangedByInput && !ImGui::IsAnyItemActive())
		m_DebugSize = m_Transform.scale * GetImageSize();

	if (!m_PosChangedByInput && !ImGui::IsAnyItemActive())
		m_DebugPos = m_Transform.translation;

	// 顯示當前 transform 狀態
	ImGui::LabelText("scale.x: ", "%.3f", m_Transform.scale.x);
	ImGui::LabelText("scale.y: ", "%.3f", m_Transform.scale.y);

	ImGui::End();
}
