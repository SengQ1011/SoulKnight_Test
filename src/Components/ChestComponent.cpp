//
// Created by QuzzS on 2025/4/25.
//

#include "Components/ChestComponent.hpp"

#include "ImagePoolManager.hpp"
#include "Override/nGameObject.hpp"
#include "RandomUtil.hpp"
#include "Util/Image.hpp"

 ChestComponent::ChestComponent(ChestType chestType, std::vector<std::string> imagePaths)
								: m_chestType(chestType), m_imagePaths(imagePaths){}


void ChestComponent::Init()
{
	auto& imagePoolManager = ImagePoolManager::GetInstance();
	for (auto& imagePath : m_imagePaths)
	{
		const auto drawable = imagePoolManager.GetImage(RESOURCE_DIR + imagePath);
		m_drawables.emplace_back(drawable);
	}
	const std::shared_ptr<nGameObject> chest = GetOwner<nGameObject>();
	if (!chest) return;
	chest->SetActive(false);
	chest->SetControlVisible(false);
	chest->SetDrawable(m_drawables[0]);

	// 預生成掉落物
	m_dropItems.clear();
	if (m_chestType == ChestType::REWARD)
	{
		int num1 = RandomUtil::RandomFloatInRange(5, 10);
		int num2 = RandomUtil::RandomFloatInRange(5, 10);
		// 生成金幣
		for (int i = 0; i < num1; i++)
		{

		}
		// 生成能量球
		for (int i = 0; i < num2; i++)
		{

		}
	}
}

void ChestComponent::Update()
{
}

// 改爲用interactableComp觸發了
void ChestComponent::HandleEvent(const EventInfo &eventInfo)
{
	if (eventInfo.GetEventType() == EventType::Collision)
	{
		const auto& collisionInfo = dynamic_cast<const CollisionEventInfo&>(eventInfo);
		HandleCollision(collisionInfo);  // 呼叫已有的碰撞處理函數
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
	if (!owner->IsActive() || !owner->IsVisible()) return;

	const std::shared_ptr<nGameObject> other = (info.GetObjectA() == owner) ? info.GetObjectB() : info.GetObjectA();

	const std::shared_ptr<CollisionComponent> otherCollisionComp = other->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (otherCollisionComp->GetCollisionLayer() & CollisionLayers_Player != 0) ChestOpened();
}

void ChestComponent::ChestOpened()
 {
 	// 若已經開過就不處理
 	if (m_currentState == ChestState::OPENED) return;
 	m_currentState = ChestState::OPENED;
 	LOG_INFO("Chest opened");

 	auto chest = GetOwner<nGameObject>();
 	if (!chest) return;

 	// 切換圖片
 	if (m_drawables.size() > 1)
 		chest->SetDrawable(m_drawables[1]); // 開啟狀態的圖片
	auto chestWorldCor = chest->GetWorldCoord();
 	// 啟用掉落物
 	for (auto& item : m_dropItems)
 	{
 		if (item)
 		{
 			item->SetActive(true);
 			item->SetControlVisible(true);

 			// 隨機丟出去一點點
 			glm::vec2 randomOffset = glm::vec2{RandomUtil::RandomFloatInRange(0.5f,30.0f),RandomUtil::RandomFloatInRange(0.5f,30.0f)};
 			item->SetWorldCoord(chestWorldCor + randomOffset);
 		}
 	}
 }
