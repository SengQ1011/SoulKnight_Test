//
// Created by AI Assistant on 2025/4/29.
//

#ifndef SHOPROOM_HPP
#define SHOPROOM_HPP

#include "EnumTypes.hpp" // 包含 PotionSize 枚舉
#include "Room/DungeonRoom.hpp"


// 前向聲明
class ShopTable;

class ShopRoom final : public DungeonRoom
{
public:
	explicit ShopRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader> &loader,
					  const std::shared_ptr<RoomObjectFactory> &room_object_factory,
					  const glm::vec2 &mapGridPos = glm::vec2(0), const RoomType roomType = RoomType::CHEST) :
		DungeonRoom(worldCoord, loader, room_object_factory, mapGridPos, roomType)
	{
	}

	~ShopRoom() override = default;

	// 重寫基類方法
	void Start(const std::shared_ptr<Character> &player) override;
	void Update() override;
	void LoadFromJSON() override;

	// 房間狀態變更處理
	void TryActivateByPlayer() override;
	void OnStateChanged() override;

	// 獲取商店桌子列表
	const std::vector<std::shared_ptr<ShopTable>> &GetShopTables() const { return m_ShopTables; }

	// 刷新所有商品
	void RefreshAllItems();

private:
	void InitializeShop();
	void CreateShopTables();
	void CreateMerchantNPC(); // 之後實作

	// 商店相關成員
	std::vector<std::shared_ptr<ShopTable>> m_ShopTables;

	bool m_ShopInitialized = false;
};

#endif // SHOPROOM_HPP
