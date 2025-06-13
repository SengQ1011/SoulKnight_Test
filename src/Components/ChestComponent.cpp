//
// Created by QuzzS on 2025/4/25.
//

#include <utility>

#include "Components/ChestComponent.hpp"

#include "Factory/RoomObjectFactory.hpp"
#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "ObserveManager/EventManager.hpp"
#include "Override/nGameObject.hpp"
#include "RandomUtil.hpp"
#include "Scene/SceneManager.hpp"
#include "Structs/EventInfo.hpp"
#include "Util/Image.hpp"

ChestComponent::ChestComponent(ChestType chestType, std::vector<std::string> imagePaths) :
	Component(ComponentType::CHEST), m_chestType(chestType), m_imagePaths(std::move(imagePaths))
{
}


void ChestComponent::Init()
{
	auto &imagePoolManager = ImagePoolManager::GetInstance();
	for (auto &imagePath : m_imagePaths)
	{
		const auto drawable = imagePoolManager.GetImage(RESOURCE_DIR + imagePath);
		m_drawables.emplace_back(drawable);
	}
	const std::shared_ptr<nGameObject> chest = GetOwner<nGameObject>();
	if (!chest)
		return;
	chest->SetActive(true);
	chest->SetControlVisible(true);
	chest->SetDrawable(m_drawables[0]);
}

void ChestComponent::Update() {}

// 改爲用interactableComp觸發了
void ChestComponent::HandleEvent(const EventInfo &eventInfo)
{
	if (eventInfo.GetEventType() == EventType::Collision)
	{
		const auto &collisionInfo = dynamic_cast<const CollisionEventInfo &>(eventInfo);
		HandleCollision(collisionInfo); // 呼叫已有的碰撞處理函數
	}
	// else if (eventInfo.GetEventType() == EventType::)
	// {
	// 	打開箱子的事件
	// 	ChestOpened();
	// }
}


void ChestComponent::HandleCollision(const CollisionEventInfo &info)
{
	const auto owner = GetOwner<nGameObject>();
	if (!owner->IsActive() || !owner->IsVisible())
		return;

	const std::shared_ptr<nGameObject> other = (info.GetObjectA() == owner) ? info.GetObjectB() : info.GetObjectA();

	const std::shared_ptr<CollisionComponent> otherCollisionComp =
		other->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (otherCollisionComp->GetCollisionLayer() & CollisionLayers_Player != 0)
		ChestOpened();
}

void ChestComponent::ChestOpened()
{
	// 若已經開過就不處理
	if (m_currentState == ChestState::OPENED)
		return;
	m_currentState = ChestState::OPENED;

	// 播放寶箱開啟音效
	AudioManager::GetInstance().PlaySFX("chest_open");

	LOG_INFO("Chest opened");

	auto chest = GetOwner<nGameObject>();
	if (!chest)
		return;

	// 切換圖片
	if (m_drawables.size() > 1)
		chest->SetDrawable(m_drawables[1]); // 開啟狀態的圖片

	ChestOpenEvent chestOpenEvent;
	chest->OnEvent(chestOpenEvent);
	chest->SetActive(false);
}
