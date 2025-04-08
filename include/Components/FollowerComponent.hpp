//
// Created by QuzzS on 2025/3/18.
//

// FollowerComponent.hpp

#ifndef FOLLOWERCOMPONENT_HPP
#define FOLLOWERCOMPONENT_HPP

#include "pch.hpp"
#include "Component.hpp"


class FollowerComponent final : public Component { //目前暫時只有武器跟隨角色,還有根據指向位置旋轉（需要事件嗎？ 我想取得指向坐標而已）
public:
	explicit FollowerComponent(const ComponentType type = ComponentType::FOLLOWER,
							   const glm::vec2& handOffset = glm::vec2(0, 0),
							   const glm::vec2& holdingOffset = glm::vec2(0, 0),
							   const float holdingRotation = 0.0f,
							   const bool isTargetMouse = false,
							   const std::shared_ptr<nGameObject>& follower = nullptr,
							   const std::shared_ptr<nGameObject>& target = nullptr) :
		Component(type), m_HandOffset(handOffset), m_HoldingOffset(holdingOffset),
		m_HoldingRotation(holdingRotation), m_UseMousePosition(isTargetMouse),
		m_Follower(follower),m_Target(target) {}

	~FollowerComponent() override = default;

	void BaseTargetRotate();
	void Update() override;

	// Setter
	void SetFollower(const std::shared_ptr<nGameObject>& follower) {m_Follower = follower;}
	void SetTarget(const std::shared_ptr<nGameObject>& target) {m_Target = target;}
	void SetHandOffset(const glm::vec2& handOffset) {m_HandOffset = handOffset;}
	void SetHoldingPosition(const glm::vec2& holdingOffset) {m_HoldingOffset = holdingOffset;}
	void IsTargetMouse(const bool isTargetMouse) {m_UseMousePosition = isTargetMouse;}
	//void SetHoldingRotation(const float& rotation) {m_HoldingRotation = rotation;}
	void SetRotationLimits(float min, float max, bool enable = true) {
		m_RotationLimits = glm::vec2(min, max);
		m_EnableRotationLimits = enable;
	}
	void SetZIndexOffset(const float zIndexOffset) {m_ZIndexOffset = zIndexOffset;}

	// 启用/禁用旋转限制
	void EnableRotationLimits(bool enable) {
		m_EnableRotationLimits = enable;
	}

	//----Getter----
	[[nodiscard]] std::shared_ptr<nGameObject> GetFollower() const { return m_Follower.lock();}
	[[nodiscard]] float GetHoldingRotation() const {return m_HoldingRotation;}
	[[nodiscard]] bool GetUseMouse() const {return m_UseMousePosition;}
	[[nodiscard]] float GetZIndexOffset() const {return m_ZIndexOffset;}


protected:
	glm::vec2 m_RotationLimits = glm::vec2(-glm::pi<float>(), glm::pi<float>()); // 旋转限制 (min, max)
	bool m_EnableRotationLimits = false;											// 是否启用旋转限制
	glm::vec2 m_HandOffset = glm::vec2(0, 0);									//不完全以世界坐標為Pivot,有一點Offset
	glm::vec2 m_HoldingOffset = glm::vec2(0, 0);								//武器也是
	float m_ZIndexOffset = 0.0f;
	float m_HoldingRotation = 0;
	bool m_UseMousePosition = false;
	std::weak_ptr<nGameObject> m_Follower;
	std::weak_ptr<nGameObject> m_Target;											//指向目標 -> 用來取位置
};

#endif //FOLLOWERCOMPONENT_HPP
