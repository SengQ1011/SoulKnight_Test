//
// Created by QuzzS on 2025/4/25.
//

#ifndef SPECIALROOM_HPP
#define SPECIALROOM_HPP

#include "Room/DungeonRoom.hpp"

class SpecialRoom final : public DungeonRoom
{
public:
	explicit SpecialRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader> &loader,
						 const std::shared_ptr<RoomObjectFactory> &room_object_factory,
						 const glm::vec2 &mapGridPos = glm::vec2(0), const RoomType roomType = RoomType::SPECIAL) :
		DungeonRoom(worldCoord, loader, room_object_factory, mapGridPos, roomType){};
	~SpecialRoom() override = default;

	// 重寫基類方法
	void Start(const std::shared_ptr<Character> &player) override;
	void Update() override;

	void LoadFromJSON() override;

private:
	void SpawnSpecialObjects();

	bool m_SpecialObjectActivated = false; // 特殊物件是否已被激活
};

#endif // SPECIALROOM_HPP
