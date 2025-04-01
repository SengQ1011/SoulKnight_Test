//
// Created by QuzzS on 2025/3/15.
//

//CollisionComponent.cpp

#include "Components/CollisionComponent.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"

void CollisionComponent::Init()
{
	auto owner = GetOwner<nGameObject>();
	if (!owner)
	{
		LOG_ERROR("Can't find owner for this component");
		return;
	}

	m_Object->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/ccccc.png"));
	m_Object->SetZIndex(100);
}


void CollisionComponent::Update()
{
	auto owner = GetOwner<nGameObject>();
	if (!owner) return;
	m_Object->SetInitialScale(m_Size);
	m_Object->m_WorldCoord = owner->m_WorldCoord + m_Offset + (m_Size/2.0f);
	m_Object->Update();
}

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
	// bool isLeft = false;
	if (owner) {
		objectPosition = owner->m_WorldCoord;
		// if (owner->m_Transform.scale.x < 0.0f) isLeft = true;
	}
	return {objectPosition + m_Offset, m_Size};
}

bool CollisionComponent::CanCollideWith(const std::shared_ptr<CollisionComponent> &other) const {
	return ((m_CollisionLayer & other->m_CollisionMask) != 0 ||
			(m_CollisionMask & other->m_CollisionLayer) != 0);
}