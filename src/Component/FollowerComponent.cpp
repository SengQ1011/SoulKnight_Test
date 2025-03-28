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
	//TODO:m_Owner坐標嗎？，還是握把坐標？ 彈道好像也可以用這個？
	//	   1. 轉角最大幅度 PI ~ -PI
	//	   2. 超過就要反轉x軸了 --> 什麽時候角色武器要反轉？ 面向目標（敵人/鼠標）->目標位置和角色位置差，當差的x<0就是要反轉了 --> 在角色實作反轉，武器被影響
	if (owner)
	{
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
	}
}


void FollowerComponent::Update()
{
	const auto owner = GetOwner<nGameObject>();
	//TODO:當角色反轉，m_HandOffset.x會被影響， m_HoldingOffset不確定，可能要實驗
	//Follower的手是m_Owner的世界坐標
	//m_Owner的轉軸是m_Owner的世界坐標-它到握把的距離
	if (owner) {
		if (const auto follower = m_Follower.lock()) {
			// 考慮跟随者的缩放方向
			const float sign = follower->m_Transform.scale.x < 0 ? -1.0f : 1.0f;

			// 应用偏移
			owner->m_WorldCoord = follower->m_WorldCoord + m_HandOffset;

			// 设置旋转和轴心点
			BaseTargetRotate(); //取得角度

			// 如果角色翻转，可能需要调整武器方向
			if (sign < 0) {
				owner->m_Transform.scale.y = -1 * std::abs(owner->m_Transform.scale.y);
			} else {
				owner->m_Transform.scale.y = std::abs(owner->m_Transform.scale.y);
			}

			owner->SetPivot(-m_HoldingOffset);
			owner->m_Transform.rotation = m_HoldingRotation;
		}
	}
}

