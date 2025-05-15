//
// Created by QuzzS on 2025/3/18.
//

//FollowerComponent.cpp

#include "Components/FollowerComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Override/nGameObject.hpp"
#include "Cursor.hpp"
#include "Util/Time.hpp"

void FollowerComponent::StartAttackAction() {
	m_attackTimer = 0.0f;
	m_attackAction = true;
}

void FollowerComponent::BaseTargetRotate() {
	// weapon
	const auto owner = GetOwner<nGameObject>();
	if (!owner){
		LOG_ERROR("FollowerComponent::BaseTargetRotate: Owner is null");
		return;
	}

	// 獲取角色的朝向
	bool facingLeft = false;
	if (const auto follower = m_Follower.lock()) {
		facingLeft = follower->m_Transform.scale.x < 0.0f;
	}

	glm::vec2 targetWorldCoord;
	bool hasTarget = false;
	// 方式1：自動攻擊
	if (auto target = m_Target.lock()){
		targetWorldCoord = target->GetWorldCoord();
		hasTarget = true;
	}
	// 方式二：鼠標追蹤
	else if (m_UseMousePosition){
		targetWorldCoord = Cursor::GetCursorWorldCoord(owner->m_Transform.scale.x);
		hasTarget = true;
	}
	// 沒有目標時，使用移動方向作為旋轉參考
	else{
		if (const auto character = m_Follower.lock()) {
			if (const auto moveComp = character->GetComponent<MovementComponent>(ComponentType::MOVEMENT)) {
				if (const glm::vec2 moveDir = moveComp->GetLastValidDirection(); glm::length(moveDir) > 0.01f) {
					// 計算基礎角度
					float baseRotation = std::atan2(moveDir.y, moveDir.x);

					// 保存基礎角度
					m_HoldingRotation = baseRotation;
				}
			}
		}
	}

	// 如果有目標，則計算朝向目標的角度
	if (hasTarget) {
		const glm::vec2 direction = targetWorldCoord - owner->m_WorldCoord;
		m_HoldingRotation = std::atan2(direction.y, direction.x);
	}

	// 對劍類武器應用額外的初始角度偏移（不管有沒有目標）
	if (m_IsSword) {
		const float prepOffset = glm::radians(m_baseRotationDegrees);
		m_HoldingRotation += facingLeft ? -prepOffset : prepOffset;
	}
	m_baseRotation = m_HoldingRotation;
}

void FollowerComponent::Update() {
	const auto owner = GetOwner<nGameObject>();

	if (owner) {
		if (const auto follower = m_Follower.lock()) {
			//跟隨角色ZIndex
			if (owner->GetZIndexType() != ZIndexType::CUSTOM)
				owner->SetZIndexType(ZIndexType::CUSTOM);
			owner->SetZIndex(follower->GetZIndex() + m_ZIndexOffset); // 置於角色前方

			// 獲取角色的朝向
			bool facingLeft = follower->m_Transform.scale.x < 0.0f;

			// 武器翻轉y軸邏輯
			owner->m_Transform.scale.y = facingLeft ? -std::abs(owner->m_Transform.scale.y) :
											std::abs(owner->m_Transform.scale.y);

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
				const float swing = glm::radians(100.0f) * std::sin(t * glm::pi<float>());  // 擺動範圍為 100 度

				// 根據角色朝向調整擺動方向
				if (facingLeft) {
					m_HoldingRotation += swing;  // 面向左側時反轉擺動方向
				} else {
					m_HoldingRotation -= swing;  // 面向右側正常擺動
				}

				if (m_attackTimer >= m_attackDuration) {
					m_attackAction = false;
				}
			}

			owner->SetPivot(-m_HoldingOffset);
			owner->m_Transform.rotation = m_HoldingRotation;
		}
	}
}