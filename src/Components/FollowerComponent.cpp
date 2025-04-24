//
// Created by QuzzS on 2025/3/18.
//

//FollowerComponent.cpp

#include "Components/FollowerComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Creature/Character.hpp"
#include "Cursor.hpp"
#include "Util/Time.hpp"

void FollowerComponent::StartAttackAction() {
	m_attackTimer = 0.0f;
	m_attackAction = true;
}


void FollowerComponent::BaseTargetRotate() {
	// weapon
	const auto owner = GetOwner<nGameObject>();
	if (!owner)
		return;

	glm::vec2 targetWorldCoord;
	// 方式1：自動攻擊
	if (auto target = m_Target.lock()){
		targetWorldCoord = target->GetWorldCoord();}
	// 方式二：鼠標追蹤
	else if (m_UseMousePosition){
		targetWorldCoord = Cursor::GetCursorWorldCoord(owner->m_Transform.scale.x);
	}
	// 沒有目標時，使用移動方向作為旋轉參考
	else{
		if (const auto character = m_Follower.lock()) {
			if (const auto moveComp = character->GetComponent<MovementComponent>(ComponentType::MOVEMENT)) {
				if (const glm::vec2 moveDir = moveComp->GetLastValidDirection(); glm::length(moveDir) > 0.01f) {
					// 計算基礎角度
					float baseRotation = std::atan2(moveDir.y, moveDir.x);

					// 拿劍類的武器：加入角色姿勢偏移
					if (m_IsSword) {
						const float prepOffset = glm::radians(35.0f); // 揮刀前的擺角
						baseRotation += (owner->m_Transform.scale.x) ? prepOffset : -prepOffset;
					}

					m_HoldingRotation = baseRotation;

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
	if (!m_IsSword) {
		owner->m_Transform.scale.y =
		(direction.x < 0.0f) ? -std::abs(owner->m_Transform.scale.y) : std::abs(owner->m_Transform.scale.y);
	}

}

void FollowerComponent::Update() {
	const auto owner = GetOwner<nGameObject>();

	if (owner) {
		if (const auto follower = m_Follower.lock()) {
			//跟隨角色ZIndex
			if (owner->GetZIndexType() != ZIndexType::CUSTOM)
				owner->SetZIndexType(ZIndexType::CUSTOM);
			owner->SetZIndex(follower->GetZIndex() + m_ZIndexOffset); // 置於角色前方

			// 应用偏移
			owner->m_WorldCoord = follower->m_WorldCoord + m_HandOffset;
			// 设置旋转和轴心点 ==> 取得角度
			BaseTargetRotate();
			// Sword:攻擊動作
			if (m_attackAction && m_IsSword) {
				m_attackTimer += Util::Time::GetDeltaTimeMs() / 1000.0f;
				// 揮劍動畫 - 設置擺動範圍
				float t = m_attackTimer / m_attackDuration;  // 動畫進度
				// 計算揮劍擺動角度，使用正弦波使得角度有起伏
				float swing = glm::radians(75.0f) * std::sin(t * glm::pi<float>());  // 擺動範圍為 60 度

				m_HoldingRotation -= swing;  // 更新武器的旋轉角度

				if (m_attackTimer >= m_attackDuration) {
					m_attackAction = false;
				}
			}


			// 若滑鼠控制，跟随者的缩放方向跟著改變
			// if (GetUseMouse())
			// {
			// 	if ((owner->m_Transform.scale.y < 0.0f && follower->m_Transform.scale.x > 0.0f) ||
			// 		(owner->m_Transform.scale.y > 0.0f && follower->m_Transform.scale.x < 0.0f))
			// 	{
			// 		follower->m_Transform.scale.x *= -1.0f;
			// 	}
			// }

			owner->SetPivot(-m_HoldingOffset);
			owner->m_Transform.rotation = m_HoldingRotation;
		}
	}
}