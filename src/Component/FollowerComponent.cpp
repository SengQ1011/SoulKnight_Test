//
// Created by QuzzS on 2025/3/18.
//

//FollowerComponent.cpp

#include "Components/FollowerComponent.hpp"
#include "Cursor.hpp"
#include "Override/nGameObject.hpp"
#include "Util/Input.hpp"

void FollowerComponent::BaseTargetRotate()
{
	const auto owner = GetOwner<nGameObject>();
	if (!owner) return;

	glm::vec2 targetWorldCoord;
	if (auto target = m_Target.lock()) {
		targetWorldCoord = target->GetWorldCoord();
	} else if (m_UseMousePosition) {
		targetWorldCoord = Cursor::GetCursorWorldCoord(owner->m_Transform.scale.x);
	} else {
		LOG_DEBUG("FollowerComponent::BaseTargetRotate");
		return;
	}
	const glm::vec2 direction = targetWorldCoord - owner->m_WorldCoord;

	auto rotation = std::atan2(direction.y, direction.x); // 计算点(x, y)与原点之间的角度[-π, π]==》旋转角度

	// 应用旋转限制
	if (m_EnableRotationLimits) {
	}

	m_HoldingRotation = rotation;

	// 武器翻轉邏輯
	owner->m_Transform.scale.y = (direction.x < 0.0f)
		? -std::abs(owner->m_Transform.scale.y): std::abs(owner->m_Transform.scale.y);
}

void FollowerComponent::Update()
{
	const auto owner = GetOwner<nGameObject>();
	//TODO:當角色反轉，m_HandOffset.x會被影響， m_HoldingOffset不確定，可能要實驗
	//Follower的手是m_Owner的世界坐標
	//m_Owner的轉軸是m_Owner的世界坐標-它到握把的距離
	if (owner) {
		if (const auto follower = m_Follower.lock()) {
			// 应用偏移
			owner->m_WorldCoord = follower->m_WorldCoord + m_HandOffset;
			// 设置旋转和轴心点
			BaseTargetRotate(); //取得角度

			// 若滑鼠控制，跟随者的缩放方向跟著改變
			if (GetUseMouse()) {
				if ((owner->m_Transform.scale.y < 0.0f && follower->m_Transform.scale.x > 0.0f) ||
					(owner->m_Transform.scale.y > 0.0f && follower->m_Transform.scale.x < 0.0f) ){
					follower->m_Transform.scale.x *= -1.0f;
				}
			}

			owner->SetPivot(-m_HoldingOffset);
			owner->m_Transform.rotation = m_HoldingRotation;
		}
	}
}

