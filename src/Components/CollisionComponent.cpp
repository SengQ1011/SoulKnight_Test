//
// Created by QuzzS on 2025/3/15.
//

//CollisionComponent.cpp

#include "Components/CollisionComponent.hpp"

#include "Util/Image.hpp"

std::shared_ptr<Util::Image> CollisionComponent::s_RedColliderImage = nullptr;
std::shared_ptr<Util::Image> CollisionComponent::s_BlueColliderImage = nullptr;
std::shared_ptr<Util::Image> CollisionComponent::s_YellowColliderImage = nullptr;


void CollisionComponent::Init()
{
	auto owner = GetOwner<nGameObject>();
	if (!owner) return;

	//初始化ColliderVisibleBox
	SetColliderBoxColor("Red");
	m_ColliderVisibleBox->SetZIndex(0.1);
	m_ColliderVisibleBox->SetZIndexType(ZIndexType::UI);
}

void CollisionComponent::SetColliderBoxColor(const std::string &color) const // Blue - 未定義， Yellow - 碰撞, Red - 正常
{
	if (!s_RedColliderImage)
		s_RedColliderImage = std::make_shared<Util::Image>(RESOURCE_DIR "/RedCollider.png");
	if (!s_BlueColliderImage)
		s_BlueColliderImage = std::make_shared<Util::Image>(RESOURCE_DIR "/BlueCollider.png");
	if (!s_YellowColliderImage)
		s_YellowColliderImage = std::make_shared<Util::Image>(RESOURCE_DIR "/YellowCollider.png");


	if (color == "Red" && s_RedColliderImage)
		m_ColliderVisibleBox->SetDrawable(s_RedColliderImage);
	if (color == "Blue" && s_BlueColliderImage)
		m_ColliderVisibleBox->SetDrawable(s_BlueColliderImage);
	if (color == "Yellow" && s_YellowColliderImage)
		m_ColliderVisibleBox->SetDrawable(s_YellowColliderImage);
}

void CollisionComponent::Update()
{
	auto owner = GetOwner<nGameObject>();
	if (!owner)
		return;
	m_ColliderVisibleBox->Update();
}

void CollisionInfo::SetCollisionNormal(const glm::vec2 &normal)
{
	if (glm::length(normal) > 0.0f)
	{
		collisionNormal = glm::normalize(normal);
	}
	else
	{
		collisionNormal = glm::vec2(1.0f, 0.0f);
	}
}

glm::vec2 CollisionInfo::GetCollisionNormal() const
{
	// 使用点积计算长度的平方，避免开方
	if (glm::dot(collisionNormal, collisionNormal) > std::numeric_limits<float>::epsilon())
	{
		return glm::normalize(collisionNormal);
	}
	return glm::vec2(0.0f); // 明确返回零向量 TODO:零向量要怎麽解決？
}

bool Rect::Intersects(const Rect &other) const
{
	constexpr float epsilon = 0.01f; // 緩衝邊界 咬到太緊不好喔
	return !(right() < other.left() + epsilon || left() > other.right() - epsilon ||
			 bottom() > other.top() - epsilon || top() < other.bottom() + epsilon);
}

Rect CollisionComponent::GetBounds() const
{
	glm::vec2 objectPosition = {0.0f, 0.0f};
	auto owner = GetOwner<nGameObject>();
	SetColliderBoxColor("Red"); //充值顔色
	if (owner)
	{
		objectPosition = owner->m_WorldCoord;
		glm::vec2 adjustedOffset = m_Offset;

		//處理可視化矩形大小和位置
		m_ColliderVisibleBox->SetInitialScale(m_Size);
		if (owner->m_Transform.scale.x < 0.0f)
		{
			adjustedOffset.x = -adjustedOffset.x; // 如果角色反向，X轴偏移需要镜像
		}
		//反向處理
		m_ColliderVisibleBox->m_WorldCoord = objectPosition + adjustedOffset;
		return {objectPosition + adjustedOffset, m_Size};
	}
	return {objectPosition + m_Offset, m_Size};
}

bool CollisionComponent::CanCollideWith(const std::shared_ptr<CollisionComponent> &other) const
{
	return ((m_CollisionLayer & other->m_CollisionMask) != 0 || (m_CollisionMask & other->m_CollisionLayer) != 0);
}

void CollisionComponent::HandleCollision(CollisionInfo &info)
{
	SetColliderBoxColor("Yellow"); //碰撞變色
}


