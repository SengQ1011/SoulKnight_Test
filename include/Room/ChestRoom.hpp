//
// Created by QuzzS on 2025/4/25.
//

#ifndef CHESTROOM_HPP
#define CHESTROOM_HPP

#include "Room/DungeonRoom.hpp"

class ChestRoom final : public DungeonRoom
{
public:
	explicit ChestRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader> &loader,
					   const std::shared_ptr<RoomObjectFactory> &room_object_factory,
					   const glm::vec2 &mapGridPos = glm::vec2(0), const RoomType roomType = RoomType::CHEST) :
		DungeonRoom(worldCoord, loader, room_object_factory, mapGridPos, roomType){};
	~ChestRoom() override = default;

	// 重寫基類方法
	void Start(const std::shared_ptr<Character> &player) override;
	void Update() override;

	void LoadFromJSON() override;

	// 房間狀態變更處理
	void TryActivateByPlayer() override;
	void OnStateChanged() override;

private:
	void SpawnChest();
	void OnChestOpened(); // 寶箱被打開時的回調

	bool m_ChestOpened = false; // 寶箱是否已被打開
};

#endif // CHESTROOM_HPP
