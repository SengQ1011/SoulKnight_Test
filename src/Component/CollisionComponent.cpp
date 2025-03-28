//
// Created by QuzzS on 2025/3/15.
//

//CollisionComponent.cpp

#include "Components/CollisionComponent.hpp"
#include "Util/Input.hpp"

void CollisionInfo::SetCollisionNormal(const glm::vec2 &normal) {
	if (glm::length(normal) > 0.0f) {
		collisionNormal = glm::normalize(normal);
	}
	else {
		collisionNormal = glm::vec2(1.0f, 0.0f);
	}
}

glm::vec2 CollisionInfo::GetCollisionNormal() const {
	// 使用点积计算长度的平方，避免开方
	if (glm::dot(collisionNormal, collisionNormal) > std::numeric_limits<float>::epsilon()) {
		return glm::normalize(collisionNormal);
	}
	return glm::vec2(0.0f); // 明确返回零向量 TODO:零向量要怎麽解決？
}

bool Rect::Intersects(const Rect &other) const {
	return !(right() < other.left() ||
			 left() > other.right() ||
			 bottom() > other.top() ||
			 top() < other.bottom());
}

Rect CollisionComponent::GetBounds() const {
	auto objectPosition = glm::vec2(0.0f);
	auto owner = GetOwner<nGameObject>();
	if (owner) {
		objectPosition = owner->m_WorldCoord;
	}
	return {objectPosition + m_Offset, m_Size};
}

void CollisionComponent::Init()
{
	auto owner = GetOwner<nGameObject>();
	if (!owner) return;
	m_Offset = -owner->GetImageSize() / 2.0f; //TODO:這裏可能需要用資料庫的資料
	m_Size = owner->GetImageSize();
	LOG_DEBUG("Collision Component Init{} {}", m_Offset, m_Size);
}

void CollisionComponent::Update()
{
	auto owner = GetOwner<nGameObject>();
	if (!owner) return;
	m_Offset = -owner->GetImageSize() / 2.0f;
	m_Size = owner->GetImageSize();
}

bool CollisionComponent::CanCollideWith(const std::shared_ptr<CollisionComponent> &other) const {
	return ((m_CollisionLayer & other->m_CollisionMask) != 0 ||
			(m_CollisionMask & other->m_CollisionLayer) != 0);
}





