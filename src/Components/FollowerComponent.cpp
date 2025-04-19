//
// Created by QuzzS on 2025/3/18.
//

//FollowerComponent.cpp

#include "Components/FollowerComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Cursor.hpp"
#include "Creature/Character.hpp"

void FollowerComponent::BaseTargetRotate()
{
	// weapon
	const auto owner = GetOwner<nGameObject>();
	if (!owner)
		return;

	glm::vec2 targetWorldCoord;
	if (auto target = m_Target.lock())
	{
		targetWorldCoord = target->GetWorldCoord();
	}
	else if (m_UseMousePosition)
	{
		targetWorldCoord = Cursor::GetCursorWorldCoord(owner->m_Transform.scale.x);
	}
	else
	{
		// 沒有目標時，使用移動方向作為旋轉參考
		if (const auto character = m_Follower.lock())
		{
			if (const auto moveComp = character->GetComponent<MovementComponent>(ComponentType::MOVEMENT))
			{
				if (const glm::vec2 moveDir = moveComp->GetLastValidDirection(); glm::length(moveDir) > 0.01f)
				{
					// 若有移動方向就以移動方向轉向
					m_HoldingRotation = std::atan2(moveDir.y, moveDir.x);

					// 同樣更新翻轉
					owner->m_Transform.scale.y = (moveDir.x < 0.0f) ? -std::abs(owner->m_Transform.scale.y)
																	: std::abs(owner->m_Transform.scale.y);
				}
			}
		}
		return;
	}
	const glm::vec2 direction = targetWorldCoord - owner->m_WorldCoord;

	const auto rotation = std::atan2(direction.y, direction.x); // 计算点(x, y)与原点之间的角度[-π, π]==》旋转角度

	// 应用旋转限制
	if (m_EnableRotationLimits)
	{
	}

	m_HoldingRotation = rotation;

	// 武器翻轉邏輯
	owner->m_Transform.scale.y =
		(direction.x < 0.0f) ? -std::abs(owner->m_Transform.scale.y) : std::abs(owner->m_Transform.scale.y);
}

void FollowerComponent::Update()
{
	const auto owner = GetOwner<nGameObject>();

	if (owner)
	{
		if (const auto follower = m_Follower.lock())
		{
			//跟隨角色ZIndex
			if (owner->GetZIndexType() != ZIndexType::CUSTOM)
				owner->SetZIndexType(ZIndexType::CUSTOM);
			owner->SetZIndex(follower->GetZIndex() + m_ZIndexOffset); // 置於角色前方

			// 应用偏移
			owner->m_WorldCoord = follower->m_WorldCoord + m_HandOffset;
			// 设置旋转和轴心点
			BaseTargetRotate(); //取得角度

			// 若滑鼠控制，跟随者的缩放方向跟著改變
			if (GetUseMouse())
			{
				if ((owner->m_Transform.scale.y < 0.0f && follower->m_Transform.scale.x > 0.0f) ||
					(owner->m_Transform.scale.y > 0.0f && follower->m_Transform.scale.x < 0.0f))
				{
					follower->m_Transform.scale.x *= -1.0f;
				}
			}

			owner->SetPivot(-m_HoldingOffset);
			owner->m_Transform.rotation = m_HoldingRotation;
		}
	}
}

void FollowerComponent::OnEnemyPositionUpdate(std::weak_ptr<Character> enemy)
{
	if (auto locked = enemy.lock())
	{
		this->SetTarget(std::static_pointer_cast<nGameObject>(locked));
	}
}


