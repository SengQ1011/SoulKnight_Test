//
// Created by QuzzS on 2025/3/16.
//

//MovementComponent.hpp

#ifndef MOVEMENTCOMPONENT_HPP
#define MOVEMENTCOMPONENT_HPP

#include "Component.hpp"
#include "Structs/CollisionComponentStruct.hpp"

class MovementComponent final : public Component
{
public:
	explicit MovementComponent(const float &speedRatio) :
		Component(ComponentType::MOVEMENT), m_SpeedRatio(speedRatio), m_currentSpeedRatio(speedRatio),
		m_Position(glm::vec2(0.0f)), m_Velocity(glm::vec2(0.0f))
	{
	}

	void Init() override;
	void Update() override;
	void HandleCollision(CollisionInfo &info) override;

	//----Getters----
	[[nodiscard]] float GetSpeedRatio() const { return m_SpeedRatio; }
	[[nodiscard]] const glm::vec2 &GetVelocity() const { return m_Velocity; }
	[[nodiscard]] const glm::vec2 &GetPosition() const { return m_Position; }
	[[nodiscard]] const glm::vec2 GetLastValidDirection() const { return m_LastValidDirection; }

	//----Setters----
	void SetPosition(const glm::vec2 &position) { m_Position = position; }
	void SetSpeedRatio(const float speedRatio) { m_SpeedRatio = speedRatio; }
	void SetVelocity(const glm::vec2 &velocity) { m_Velocity = velocity; }
	void SetOnIce(bool isOnIce) { m_IsOnIce = isOnIce; }
	// 控制移動
	void SetDesiredDirection(const glm::vec2 &direction) { m_DesiredDirection = direction; }

private:
	float m_SpeedRatio; // 基本移動速度係數
	float m_currentSpeedRatio = 1.0f; // 當前速度倍率
	float m_SpeedEffectDuration = 0.0f; // 剩餘的加減速時間
	bool m_IsOnIce = false; // 是否在冰面
	glm::vec2 m_Position; // 當前位置
	glm::vec2 m_DesiredDirection; // 移動方向向量（輸入）
	glm::vec2 m_Velocity; // 當前速度向量
	// 冰面專用參數()
	float m_IceAcceleration = 4.0f; // 冰面加速度--》值越小，加速越慢（需要更長時間提速）
	float m_IceFriction = 15.0f; // 冰面摩擦力==》值越大，減速越快（滑行距離短）
	float m_TurnSmoothness = 10.0f; // 轉向平滑係數（越大=>越難轉向）
	float m_LateralFriction = 5.0f; // 橫向摩擦力 (斜向移動時用)
	// 方向系統
	glm::vec2 m_LastValidDirection; // 記錄上次實際移動方向（用於慣性計算）
	float m_DirectionMemoryThreshold = 0.3f; // 記錄方向的最低速度
	float m_DecelerationCurve = 1.5f; // 減速曲線強度 (值越大，高速時減速越快)

	// 記錄每個方向的碰撞狀態
	struct ContactState
	{
		bool inContactX = false;
		bool inContactY = false;
		glm::vec2 contactNormal = glm::vec2(0.0f);
		float contactTime = 0.0f;
		const float contactTimeout = 0.1f; // 接觸狀態持續時間（秒）
	};

	ContactState m_ContactState;
};


#endif //MOVEMENTCOMPONENT_HPP
