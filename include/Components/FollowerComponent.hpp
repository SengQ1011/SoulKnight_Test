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
	explicit FollowerComponent(const std::string& name = "followerComponent",
							   const glm::vec2& handOffset = glm::vec2(0, 0),
							   const glm::vec2& holdingOffset = glm::vec2(0, 0),
							   const float holdingRotation = 0.0f,
							   const bool isTargetMouse = false,
							   const std::shared_ptr<nGameObject>& follower = nullptr,
							   const std::shared_ptr<nGameObject>& target = nullptr) :
		Component(name), m_HandOffset(handOffset), m_HoldingOffset(holdingOffset),
		m_HoldingRotation(holdingRotation), m_UseMousePosition(isTargetMouse),
		m_Follower(follower),m_Target(target) {}

	~FollowerComponent() override = default;

	void BaseTargetRotate();
	void Start(); //TODO:錦鑫 如果不想一開始一堆Set,可以這裏增加參數然後一次過Set
	void Update() override;

	// Setter
	void SetFollower(const std::shared_ptr<nGameObject>& follower) {m_Follower = follower;}
	void SetTarget(const std::shared_ptr<nGameObject>& target) {m_Target = target;}
	void SetHandOffset(const glm::vec2& handOffset) {m_HandOffset = handOffset;}
	void SetHoldingPosition(const glm::vec2& holdingOffset) {m_HoldingOffset = holdingOffset;}
	void IsTargetMouse(const bool isTargetMouse) {m_UseMousePosition = isTargetMouse;}
	//void SetHoldingRotation(const float& rotation) {m_HoldingRotation = rotation;}

	[[nodiscard]] float GetHoldingRotation() const {return m_HoldingRotation;}
protected:
	glm::vec2 m_HandOffset = glm::vec2(0, 0); //不完全以世界坐標為Pivot,有一點Offset
	glm::vec2 m_HoldingOffset = glm::vec2(0, 0); //武器也是
	float m_HoldingRotation = 0;
	bool m_UseMousePosition = false;
	std::shared_ptr<nGameObject> m_Follower = nullptr;
	std::shared_ptr<nGameObject> m_Target = nullptr; //指向目標 -> 用來取位置
};

#endif //FOLLOWERCOMPONENT_HPP
