//
// Created by QuzzS on 2025/3/16.
//

//MovementComponent.cpp

#include "Components/MovementComponent.hpp"
#include "Attack/EffectAttack.hpp"
#include "Components/CollisionComponent.hpp"
#include "Structs/KnockOffEventInfo.hpp"
#include "Structs/SpeedChangeEventInfo.hpp"
#include "Util/Time.hpp"


void MovementComponent::Init() {}

void MovementComponent::Update()
{
	const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	const auto owner = GetOwner<nGameObject>();
	if (owner)
		m_Position = owner->m_WorldCoord;

	// 檢查是否有加速/減速效果
	if (m_SpeedEffectDuration > 0.0f)
	{
		m_SpeedEffectDuration -= deltaTime;
		if (m_SpeedEffectDuration <= 0.0f)
		{
			m_SpeedEffectDuration = 0.0f;
			if (m_freeze) m_freeze = false;
		}
	}

	// 處理平滑恢復(平滑过渡回原始速度)
	if (m_SpeedEffectDuration <= 0.0f && std::abs(m_currentSpeedRatio - m_SpeedRatio) > 0.01f)
	{
		m_currentSpeedRatio = glm::mix(m_currentSpeedRatio, m_SpeedRatio, deltaTime * 2.0f);
		if (std::abs(m_currentSpeedRatio - m_SpeedRatio) < 0.01f)
		{
			m_currentSpeedRatio = m_SpeedRatio;
		}
	}

	if (m_freeze) return;

	// ===== 移動邏輯核心 =====
	constexpr float baseSpeed = 120.0f; // 基础速度
	const float effectiveMaxSpeed = baseSpeed * m_currentSpeedRatio;

	// 確保輸入方向已正規化
	glm::vec2 inputDir =
		(glm::length(m_DesiredDirection) > 0.01f) ? glm::normalize(m_DesiredDirection) : glm::vec2(0.0f);

	// 當前速度計算
	float currentSpeed = glm::length(m_Velocity);
	glm::vec2 currentDir = currentSpeed > 0.01f ? m_Velocity / currentSpeed : m_LastValidDirection;

	// 冰面参数查看hpp
	if (m_IsOnIce) {
		// 分軸速度處理
		glm::vec2 velocityX = glm::vec2(m_Velocity.x, 0.0f);
		glm::vec2 velocityY = glm::vec2(0.0f, m_Velocity.y);
		float speedX = glm::length(velocityX);
		float speedY = glm::length(velocityY);

		// 更新方向記憶
		if (currentSpeed > m_DirectionMemoryThreshold)
		{
			m_LastValidDirection = currentDir;
		}

		// 斜向移動特殊處理
		if (std::abs(inputDir.x) > 0.1f && std::abs(inputDir.y) > 0.1f)
		{
			// 斜向移動時加強垂直方向減速
			if (speedY > 0.1f)
			{
				float decelY = m_IceFriction * 1.8f * deltaTime;
				velocityY -= glm::normalize(velocityY) * decelY;
			}
			// 水準方向正常減速
			if (speedX > 0.1f)
			{
				float decelX = m_LateralFriction * deltaTime;
				velocityX -= glm::normalize(velocityX) * decelX;
			}
			m_Velocity = velocityX + velocityY;
		}

		// 輸入處理
		if (glm::length(inputDir) > 0.01f)
		{
			// 急轉檢測（反向輸入）
			if (glm::dot(m_LastValidDirection, inputDir) < -0.7f)
			{
				float brakePower = m_IceFriction * 8.0f * m_currentSpeedRatio * deltaTime;
				m_Velocity -= m_Velocity * brakePower;
			}

			// 限制轉向速度
			float maxTurnSpeed = effectiveMaxSpeed * 0.6f;
			if (currentSpeed > maxTurnSpeed)
			{
				m_Velocity = m_LastValidDirection * maxTurnSpeed;
			}

			// 方向混合（平滑轉向）
			glm::vec2 targetDir =
				glm::mix(m_LastValidDirection, inputDir, std::min(1.0f, m_TurnSmoothness * deltaTime));
			targetDir = glm::normalize(targetDir);

			// 應用加速度
			const glm::vec2 targetVel = targetDir * effectiveMaxSpeed;
			const glm::vec2 accel = (targetVel - m_Velocity) * m_IceAcceleration * m_currentSpeedRatio * deltaTime;
			m_Velocity += accel;
		}
		// 無輸入時的減速
		else
		{
			if (currentSpeed > 0.01f)
			{
				// 曲線減速公式
				float decel =
					m_IceFriction * deltaTime * (1.0f + pow(currentSpeed / effectiveMaxSpeed, m_DecelerationCurve));

				m_Velocity -= m_LastValidDirection * decel;

				if (glm::length(m_Velocity) < 0.1f)
				{
					m_Velocity = glm::vec2(0.0f);
					m_LastValidDirection = glm::vec2(0.0f);
				}
			}
		}
	}
	// 普通地面移動
	else
	{
		m_Velocity = inputDir * effectiveMaxSpeed;
		if (glm::length(inputDir) > 0.01f)
		{
			m_LastValidDirection = inputDir;
		}
	}

	// 更新接觸狀態超時
	if (m_ContactState.inContactX || m_ContactState.inContactY) {
		m_ContactState.contactTime -= deltaTime;
		if (m_ContactState.contactTime <= 0.0f)
		{
			m_ContactState.inContactX = false;
			m_ContactState.inContactY = false;
		}
	}

	// 根據接觸狀態限制速度
	if (m_ContactState.inContactX && std::abs(m_ContactState.contactNormal.x) > 0.01f) {
		if (m_Velocity.x * m_ContactState.contactNormal.x < 0) {
			m_Velocity.x = 0.0f; // 停止X方向的運動
		} else{
			m_ContactState.inContactX = false;
		}
	}

	if (m_ContactState.inContactY && std::abs(m_ContactState.contactNormal.y) > 0.01f)
	{
		if (m_Velocity.y * m_ContactState.contactNormal.y < 0) {
			m_Velocity.y = 0.0f; // 停止Y方向的運動
		} else {
			m_ContactState.inContactY = false;
		}
	}

	// 极速时限制（防止溢出）
	if (glm::length(m_Velocity) > effectiveMaxSpeed) {
		m_Velocity = glm::normalize(m_Velocity) * effectiveMaxSpeed;
	}

	// 極小速度歸零（防止抖動）
	if (glm::length(m_Velocity) < 0.5f)
	{
		m_Velocity = glm::vec2(0.0f);
		m_ContactState.inContactX = false;
		m_ContactState.inContactY = false;
	}

	// 更新位置
	glm::vec2 totalVelocity = m_Velocity + m_ImpulseVelocity;
	m_Position += totalVelocity * deltaTime;
	if (owner)
		owner->m_WorldCoord = m_Position;

	// ImpulseVelocity 自然衰減
	if (glm::length(m_ImpulseVelocity) > 0.01f)
	{
		m_ImpulseVelocity = glm::mix(m_ImpulseVelocity, glm::vec2(0.0f), m_ImpulseDamping * deltaTime);
	}
	else
	{
		m_ImpulseVelocity = glm::vec2(0.0f); // 避免浮點數誤差導致抖動
	}


	// 若owner有attackComp且有目標：跟隨目標方向
	if(auto attackComp = owner->GetComponent<AttackComponent>(ComponentType::ATTACK);
		attackComp && attackComp->HasTarget())
	{
		glm::vec2 targetDirection = attackComp->GetTarget().lock()->GetWorldCoord() - owner->m_WorldCoord;
		owner->m_Transform.scale.x =
		(targetDirection.x < 0.0f) ? -std::abs(owner->m_Transform.scale.x) : std::abs(owner->m_Transform.scale.x);
	}
	// 若沒有目標：調整面朝方向
	else{
		owner->m_Transform.scale.x =
			(m_LastValidDirection.x < 0.0f) ? -std::abs(owner->m_Transform.scale.x) : std::abs(owner->m_Transform.scale.x);
	}

}
void MovementComponent::HandleEvent(const EventInfo &eventInfo)
{
	switch (eventInfo.GetEventType())
	{
		case EventType::Collision:
		{
			const auto& collisionInfo = dynamic_cast<const CollisionEventInfo&>(eventInfo);
			HandleCollision(collisionInfo);
			break;
		}
		case EventType::KnockOff:
		{
			// LOG_DEBUG("KnockOff");
			const auto& knockOffInfo = dynamic_cast<const KnockOffEventInfo&>(eventInfo);
			SetImpulseVelocity(knockOffInfo.impulseVelocity);
			break;
		}
		case EventType::SpeedChanged:
		{
			// LOG_DEBUG("SpeedChanged");
			const auto& speedChangedInfo = dynamic_cast<const SpeedChangeEventInfo&>(eventInfo);
			SetCurrentSpeedRatio(speedChangedInfo.speedRatio);
			m_SpeedEffectDuration = speedChangedInfo.durationTime; // 设置持续时间
			break;
		}
		default:
			break;
	}
}

std::vector<EventType> MovementComponent::SubscribedEventTypes() const
{
	return {
		EventType::Collision,
		EventType::KnockOff,
		EventType::SpeedChanged
	};
}


void MovementComponent::HandleCollision(const CollisionEventInfo& eventInfo)
{
	const auto owner = GetOwner<nGameObject>();
	if (owner)
	{
		m_Position = owner->m_WorldCoord;
	}

	const std::shared_ptr<nGameObject> other = (eventInfo.GetObjectA() == owner) ? eventInfo.GetObjectB() : eventInfo.GetObjectA();

	if (const std::shared_ptr<CollisionComponent> otherCollider =
			other->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		otherCollider && otherCollider->IsTrigger())
	{
		return;
	}

	const glm::vec2 collisionNormal = eventInfo.GetCollisionNormal();
	auto penetration = eventInfo.penetration;
	ResolveOverlap(collisionNormal, penetration);
}

void MovementComponent::ResolveOverlap(glm::vec2 collisionNormal, float penetration)
{
	const auto owner = GetOwner<nGameObject>();
	if (!owner) return;

	// 設置接觸狀態
	if (std::abs(collisionNormal.x) > 0.01f)
	{
		m_ContactState.inContactX = true;
	}
	if (std::abs(collisionNormal.y) > 0.01f)
	{
		m_ContactState.inContactY = true;
	}

	if (m_ContactState.inContactX || m_ContactState.inContactY)
	{
		m_ContactState.contactNormal = collisionNormal;
		m_ContactState.contactTime = m_ContactState.contactTimeout;
	}

	// 調整位置
	m_Position += collisionNormal * penetration;

	// 立即更新物體位置
	if (owner)
		owner->m_WorldCoord = m_Position;
}