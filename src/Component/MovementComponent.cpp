//
// Created by QuzzS on 2025/3/16.
//

//MovementComponent.cpp

#include "Components/MovementComponent.hpp"
#include "Components/CollisionComponent.hpp"
#include "Util/Time.hpp"

void MovementComponent::Init() {
	auto owner = GetOwner<nGameObject>();
	if (owner) {

	}
}


void MovementComponent::Update() {
	const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	const auto owner = GetOwner<nGameObject>();
	// 更新接觸狀態超時
	if (m_ContactState.inContactX || m_ContactState.inContactY) {
		m_ContactState.contactTime -= deltaTime;
		if (m_ContactState.contactTime <= 0.0f) {
			// 接觸狀態超時，重置
			m_ContactState.inContactX = false;
			m_ContactState.inContactY = false;
		}
	}
	// 根據接觸狀態限制加速度
	if (m_ContactState.inContactX && std::abs(m_ContactState.contactNormal.x) > 0.01f) {
		// 如果加速度方向與碰撞法線相反，則將該方向加速度歸零
		if (m_Acceleration.x * m_ContactState.contactNormal.x < 0) {
			m_Acceleration.x = 0.0f;
		} else {
			// 加速度方向不再朝向碰撞表面，可以結束該方向的接觸狀態
			m_ContactState.inContactX = false;
		}
	}
	constexpr float accelerationFactor = 5000.0f;
	if (owner) m_Position = owner->m_WorldCoord;

	// 加速度受SpeedRatio影響 TODO:(速度buff是否要影響達到最大速度的時間？)
	m_Velocity += m_Acceleration * accelerationFactor * m_SpeedRatio * deltaTime;

	//摩擦力
	constexpr float friction = 5000.0f; //TODO:摩擦力可能是變數 10-15， 目前爲了及時提速降速先超大值
	const float frictionMagnitude = friction * deltaTime;

	if (m_Acceleration.x == 0.0f) //摩擦力啓動！
	{
		if (const float speedMagnitude = std::abs(m_Velocity.x); speedMagnitude > 0.0f) {
			const float newSpeed = std::max(0.0f, speedMagnitude - frictionMagnitude);
			if (speedMagnitude > 0.01f) m_Velocity.x *= (newSpeed / speedMagnitude);
			else m_Velocity.x = 0.0f; //速度太小，直接設零
		}
	}
	if (m_Acceleration.y == 0.0f) {
		if (const float speedMagnitude = std::abs(m_Velocity.y); speedMagnitude > 0.0f) {
			const float newSpeed = std::max(0.0f, speedMagnitude - frictionMagnitude);
			if (speedMagnitude > 0.01f) m_Velocity.y *= (newSpeed / speedMagnitude);
			else m_Velocity.y = 0.0f; //速度太小，直接設零
		}
	}

	if (glm::length(m_Velocity) < 0.01f) { //速度極小，防止抖動，提前結束
		m_Velocity = glm::vec2(0.0f);
		return;
	}
	//取得基礎速度的方向和大小
	const glm::vec2 direction = glm::normalize(m_Velocity);
	const float baseSpeed = glm::length(m_Velocity);

	//速度受到速度比率影響 同時限速
	const float effectiveSpeed = std::min(baseSpeed, m_MaxSpeed) * m_SpeedRatio;

	m_Velocity = direction * effectiveSpeed;
	m_Position += m_Velocity * deltaTime;

	m_Acceleration = glm::vec2(0.0f); //重置當前加速度
	if (owner) owner->m_WorldCoord = m_Position;
}

void MovementComponent::HandleCollision(CollisionInfo &info)
{
	const auto owner = GetOwner<nGameObject>();
	if (owner) {m_Position = owner->m_WorldCoord;}
	//TODO:我似乎已經在其他地方確認了，這裏還需要嗎？
	if (info.GetObjectA() != owner && info.GetObjectB() != owner) {
		return;
	}

	const std::shared_ptr<nGameObject> other = (info.GetObjectA() == owner) ? info.GetObjectB() : info.GetObjectA();

	if (const std::shared_ptr<CollisionComponent> otherCollider = other->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		otherCollider && otherCollider->IsTrigger())
	{
		return;
	}

	const glm::vec2 collisionNormal = info.GetCollisionNormal();

	// 設置接觸狀態
	if (std::abs(collisionNormal.x) > 0.01f) {
		m_ContactState.inContactX = true;
	}
	if (std::abs(collisionNormal.y) > 0.01f) {
		m_ContactState.inContactY = true;
	}

	if (m_ContactState.inContactX || m_ContactState.inContactY) {
		m_ContactState.contactNormal = collisionNormal;
		m_ContactState.contactTime = m_ContactState.contactTimeout;
	}

	// 調整位置
	m_Position += collisionNormal * info.penetration;

	// 歸零速度
	if (std::abs(collisionNormal.x) >  0.01f) m_Velocity.x = 0.0f;
	if (std::abs(collisionNormal.y) >  0.01f) m_Velocity.y = 0.0f;

	// 立即更新物體位置
	if (owner) owner->m_WorldCoord = m_Position;
}