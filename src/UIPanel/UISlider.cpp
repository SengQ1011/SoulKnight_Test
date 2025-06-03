//
// Created by QuzzS on 2025/5/22.
//

#include "UIPanel/UISlider.hpp"

#include "Structs/PositionChangedEvent.hpp"
#include "Tool/Tool.hpp"
#include "UIPanel/UIButton.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

UISlider::UISlider(const std::function<float()>& listenFunction,
					  const std::shared_ptr<nGameObject>& track,
					  const glm::vec2& borderWidth,
					  bool hasButton,
					  const std::string& buttonImage,
					  const std::function<void(float)>& trackFunction) :
	m_ListenFunction(listenFunction),
	m_TrackFunction(trackFunction),
	m_Track(track),
	m_BorderWidth(borderWidth)
{
	// 沒有按鈕就直接返回
	if (!hasButton) return;

	// 定義按鈕拖曳時的 callback（會每幀呼叫）
	std::function<void()> button_function = [this]()
	{
		glm::vec2 limitedMove = Tool::GetMouseCoord();

		// 限制滑塊 X 軸在 groove 範圍內
		const float grooveCenterX = this->m_Transform.translation.x;
		const float halfWidth = this->GetScaledSize().x * 0.5f - m_BorderWidth.x;
		limitedMove.x = std::clamp(limitedMove.x, grooveCenterX - halfWidth, grooveCenterX + halfWidth);

		// 固定滑塊 Y 軸與 groove 對齊
		limitedMove.y = this->m_Transform.translation.y;

		// 更新滑塊位置並發出事件
		m_Button->m_Transform.translation = limitedMove;
		const PositionChangedEvent eventInfo(m_Button->m_Transform.translation);
		this->OnEvent(eventInfo);

		// 設定為正在拖曳狀態
		isFollowButton = true;
	};

	// 初始化 Button 並設定圖片與縮放
	m_Button = std::make_shared<UIButton>(button_function, true);
	m_Button->SetDrawable(ImagePoolManager::GetInstance().GetImage(buttonImage));
}

void UISlider::Start()
{
	// 設定 Track 的 Pivot（將中心改為左邊起點）
	m_Track->SetZIndex(this->GetZIndex()+1.0f);
	m_Track->SetPivot(glm::vec2(m_Track->GetPivot().x - m_Track->GetImageSize().x / 2.0f, 0.0f));

	if (!m_Button) return;
	m_Button->m_Transform.translation = m_Track->m_Transform.translation + glm::vec2(m_Track->GetScaledSize().x * 0.5f, 0.0f);
	// m_Button->m_Transform.scale = glm::vec2(16.0f, 48.0f) / m_Button->GetImageSize(); //TODO:可能要變數而不是常數
	m_Button->SetZIndex(m_Track->GetZIndex()+1.0f);
}


void UISlider::Update()
{
	nGameObject::Update();

	// [1] 判斷是否滑鼠放開，若放開則停止拖曳
	if (isFollowButton && Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB))
		isFollowButton = false;

	// [2] 拖曳中則不跟隨 ListenFunction 值更新畫面
	if (isFollowButton) return;

	// [3] 取得目前音量/值（0.0 ~ 1.0）
	float currentValue = 0.0f;
	if (m_ListenFunction)
		currentValue = m_ListenFunction();

	// [4] 計算 groove 左右邊界
	const float grooveLeft = this->m_Transform.translation.x - this->GetScaledSize().x * 0.5f + m_BorderWidth.x;
	const float grooveRight = this->m_Transform.translation.x + this->GetScaledSize().x * 0.5f - m_BorderWidth.x;

	// [5] 更新 Track 顯示長度
	if (m_Track)
	{
		m_Track->m_Transform.translation = glm::vec2(grooveLeft, this->m_Transform.translation.y);
		m_Track->m_Transform.scale = glm::vec2((grooveRight - grooveLeft) * currentValue, this->GetScaledSize().y - m_BorderWidth.y) / m_Track->GetImageSize();
	}

	// [6] 更新 Button 位置（對齊 track 右邊）
	if (m_Button)
	{
		m_Button->m_Transform.translation = m_Track->m_Transform.translation + glm::vec2(m_Track->GetScaledSize().x, 0.0f);
		// m_Button->m_Transform.scale = glm::vec2(16.0f, 48.0f) / m_Button->GetImageSize();
	}

	DrawDebugUI();
}

// ==========================
// 接收事件分派
// ==========================
void UISlider::OnEventReceived(const EventInfo &eventInfo)
{
	switch (eventInfo.GetEventType())
	{
	case EventType::PositionChanged:
	{
		const auto button_position = dynamic_cast<const PositionChangedEvent& >(eventInfo);
		TrackFollowButton(button_position);
		break;
	}
	case EventType::ValueChanged:
		LOG_DEBUG("Value changed");
		break;
	default:
		break;
	}
}

// ==========================
// 滑塊位置對應數值並執行函式
// ==========================
void UISlider::TrackFollowButton(const PositionChangedEvent &eventInfo)
{
	const float grooveLeft = this->m_Transform.translation.x - this->GetScaledSize().x * 0.5f + m_BorderWidth.x;
	const float grooveRight = this->m_Transform.translation.x + this->GetScaledSize().x * 0.5f - m_BorderWidth.x;

	const float buttonPosition = eventInfo.m_NewPosition.x;
	const float maxValue = grooveRight - grooveLeft;
	const float newValue = buttonPosition - grooveLeft;

	// 更新 Track 長度（填色長度）
	m_Track->m_Transform.scale.x = newValue / m_Track->GetImageSize().x;
	m_Track->m_Transform.scale.y = (this->GetScaledSize().y  - m_BorderWidth.y) / m_Track->GetImageSize().y;

	// 通知外部 callback（回傳值為 0~1）
	if (m_TrackFunction)
		m_TrackFunction(newValue / maxValue);
}



