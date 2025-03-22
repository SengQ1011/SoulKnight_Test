//
// Created by QuzzS on 2025/3/16.
//

//MovementComponent.hpp

#ifndef MOVEMENTCOMPONENT_HPP
#define MOVEMENTCOMPONENT_HPP

#include "pch.hpp"

#include "Component.hpp"
#include "Override/nGameObject.hpp"

class MovementComponent final : public Component {
public:
	explicit MovementComponent(const std::string& name = "movementComponent",
							   const float& speedRatio = 1.0f,
							   const float& maxSpeed = 100.0f,
							   const glm::vec2& acceleration = glm::vec2(0.0f),
							   const glm::vec2& velocity = glm::vec2(0.0f),
							   const glm::vec2& position = glm::vec2(0.0f))
		: Component(name), m_SpeedRatio(speedRatio), m_MaxSpeed(maxSpeed), m_Acceleration(acceleration),
		  m_Velocity(velocity), m_Position(position) {}

	void Update() override;
	void HandleCollision(CollisionInfo &info) override;

	// Getters
	[[nodiscard]] float SetSpeedRatio() const { return m_SpeedRatio; }
	[[nodiscard]] float GetMaxSpeed() const { return m_MaxSpeed; }
	[[nodiscard]] const glm::vec2& GetAcceleration() const { return m_Acceleration; }
	[[nodiscard]] const glm::vec2& GetVelocity() const { return m_Velocity; }
	[[nodiscard]] const glm::vec2& GetPosition() const { return m_Position; }

	// Setters
	void SetPosition(const glm::vec2& position) { m_Position = position; }
	void SetSpeedRatio(const float speedRatio) { m_SpeedRatio = speedRatio; }
	void SetMaxSpeed(const float maxSpeed) { m_MaxSpeed = maxSpeed; }
	void SetVelocity(const glm::vec2& velocity) {m_Velocity = velocity;}
	void SetAcceleration(const glm::vec2& acceleration) { m_Acceleration = acceleration; }

private:
	//TODO: AccelerationFactor和 Friction可能也要是成員變數，這樣可以實現滑行等操作
	float m_SpeedRatio;			// 基本移動速度係數
	float m_MaxSpeed;			// 最大速度限制
	glm::vec2 m_Acceleration;	// 加速度
	glm::vec2 m_Velocity;		// 當前速度向量
	glm::vec2 m_Position;		// 當前位置

	// 記錄每個方向的碰撞狀態
	struct ContactState {
		bool inContactX = false;
		bool inContactY = false;
		glm::vec2 contactNormal = glm::vec2(0.0f);
		float contactTime = 0.0f;
		const float contactTimeout = 0.1f; // 接觸狀態持續時間（秒）
	};

	ContactState m_ContactState;
};
#endif //MOVEMENTCOMPONENT_HPP
