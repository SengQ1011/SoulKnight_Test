//
// Created by QuzzS on 2025/4/25.
//

#include "Components/ChestComponent.hpp"
#include "Override/nGameObject.hpp"
#include "Util/Image.hpp"

void ChestComponent::Init()
{
	const auto drawable0 = std::make_shared<Util::Image>(RESOURCE_DIR"/IcePlains/object_chest_0.png");
	const auto drawable1 = std::make_shared<Util::Image>(RESOURCE_DIR"/IcePlains/object_chest_1.png");
	m_drawables.emplace_back(drawable0);
	m_drawables.emplace_back(drawable1);
	const std::shared_ptr<nGameObject> chest = GetOwner<nGameObject>();
	if (!chest) return;
	chest->SetActive(false);
	chest->SetVisible(false);
	chest->SetDrawable(drawable0);
}

void ChestComponent::Update()
{
}

void ChestComponent::HandleCollision(CollisionInfo &info)
{
	const auto owner = GetOwner<nGameObject>();
	if (!owner->IsActive() || !owner->IsVisible()) return;

	const std::shared_ptr<nGameObject> other = (info.GetObjectA() == owner) ? info.GetObjectB() : info.GetObjectA();

	const std::shared_ptr<CollisionComponent> otherCollisionComp = other->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (otherCollisionComp->GetCollisionLayer() & CollisionLayers_Player != 0) ChestOpened();
}

void ChestComponent::ChestOpened()
{

}



