//
// DungeonRoom 碰撞優化實現 - 整合現有碰撞系統
//

#include "Components/CollisionComponent.hpp"
#include "Room/CollisionOptimizer.hpp"
#include "Room/DungeonRoom.hpp"
#include "RoomObject/WallObject.hpp"
#include "Scene/SceneManager.hpp"
#include "Structs/CollisionComponentStruct.hpp" // 使用現有的碰撞層定義

// 在 DungeonRoom 中添加碰撞優化功能的實現
void DungeonRoom::OptimizeWallCollisions()
{
	// 1. 創建碰撞優化器
	CollisionOptimizer optimizer;

	// 2. 分析房間物件並生成優化的碰撞區域
	auto optimizedRegions = optimizer.OptimizeWallCollisions(m_RoomObjects, m_RoomSpaceInfo.m_TileSize,
															 m_RoomSpaceInfo.m_WorldCoord, RoomConstants::GRID_SIZE);

	// 3. 移除原有牆壁的碰撞組件
	RemoveWallCollisionComponents();

	// 4. 創建優化的碰撞箱
	auto optimizedColliders = CreateOptimizedColliders(optimizedRegions);

	// 5. 將優化的碰撞箱添加到房間（AddRoomObject 會自動註冊到管理器）
	for (auto &collider : optimizedColliders)
	{
		AddRoomObject(collider); // 這裡會自動註冊到碰撞管理器，不需要手動註冊
	}
}

void DungeonRoom::RemoveWallCollisionComponents()
{
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene)
		return;

	for (auto &obj : m_RoomObjects)
	{
		// 檢查是否為牆壁物件
		if (obj->GetClassName() != "WallObject")
			continue;

		auto collisionComp = obj->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		if (!collisionComp)
			continue;

		// 從 PendingObjects 中移除 visibleBox
		RemoveVisibleBoxFromPendingObjects(collisionComp, scene);

		// 從碰撞管理器中註銷物件
		m_CollisionManager->UnregisterNGameObject(obj);
	}
}

void DungeonRoom::RemoveVisibleBoxFromPendingObjects(const std::shared_ptr<CollisionComponent> &collisionComp,
													 const std::shared_ptr<Scene> &scene)
{
	const auto visibleBox = collisionComp->GetVisibleBox();
	if (!visibleBox)
		return;

	auto &pendingObjects = scene->GetPendingObjects();
	auto it = std::remove_if(pendingObjects.begin(), pendingObjects.end(),
							 [&visibleBox](const std::weak_ptr<nGameObject> &weakPtr)
							 {
								 if (auto sharedPtr = weakPtr.lock())
								 {
									 return sharedPtr.get() == visibleBox.get();
								 }
								 return false; // 移除過期的 weak_ptr
							 });

	if (it != pendingObjects.end())
	{
		pendingObjects.erase(it, pendingObjects.end());
	}
}

std::vector<std::shared_ptr<nGameObject>>
DungeonRoom::CreateOptimizedColliders(const std::vector<CollisionRect> &regions)
{
	std::vector<std::shared_ptr<nGameObject>> colliders;
	auto &img = ImagePoolManager::GetInstance();

	for (size_t i = 0; i < regions.size(); ++i)
	{
		const auto &region = regions[i];

		// 創建一個不可見的碰撞箱物件
		auto collider = std::make_shared<WallObject>("optimized_wall_collider_" + std::to_string(i));

		// 設置物件的世界座標為房間中心（與 LobbyRoom 一致）
		collider->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);
		collider->SetDrawable(img.GetImage(RESOURCE_DIR "/empty.png"));

		// 添加碰撞組件，使用現有的碰撞系統
		auto collisionComp = collider->AddComponent<CollisionComponent>(ComponentType::COLLISION);
		if (collisionComp)
		{
			// 設置碰撞箱大小
			collisionComp->SetSize(region.size);

			// 計算相對於房間中心的偏移（與 LobbyRoom 的做法一致）
			glm::vec2 offset = region.worldPos - m_RoomSpaceInfo.m_WorldCoord;
			collisionComp->SetOffset(offset);

			// 使用現有的碰撞層枚舉
			collisionComp->SetCollisionLayer(CollisionLayers_Terrain);

			// 設置碰撞遮罩 - 與所有移動物體碰撞
			collisionComp->AddCollisionMask(CollisionLayers_Player);
			collisionComp->AddCollisionMask(CollisionLayers_Enemy);
			collisionComp->AddCollisionMask(CollisionLayers_Player_Projectile);
			collisionComp->AddCollisionMask(CollisionLayers_Enemy_Projectile);
			collisionComp->AddCollisionMask(CollisionLayers_Player_EffectAttack);
			collisionComp->AddCollisionMask(CollisionLayers_Enemy_EffectAttack);

			// 設置為靜態碰撞體（不是觸發器）
			collisionComp->SetTrigger(false);
			collisionComp->SetActive(true);
		}

		// 設置為不可見（只有碰撞功能）
		collider->SetControlVisible(false);

		colliders.push_back(collider);
	}

	return colliders;
}

// 注意：OptimizeWallCollisions() 應該在 DungeonRoom::Start() 中調用
// 而不是在這裡重複定義 Start() 方法
