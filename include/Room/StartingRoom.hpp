//
// Created by QuzzS on 2025/4/24.
//

#ifndef STARTINGROOM_HPP
#define STARTINGROOM_HPP

#include "Room/DungeonRoom.hpp"

class StartingRoom final : public DungeonRoom {
public:
	explicit StartingRoom(
		const glm::vec2 worldCoord,
		const std::shared_ptr<Loader>& loader,
		const std::shared_ptr<RoomObjectFactory>& room_object_factory,
		const glm::vec2& mapGridPos = glm::vec2(0),
		const RoomType roomType = RoomType::STARTING)
		: DungeonRoom(worldCoord, loader, room_object_factory,mapGridPos,roomType) {};
	~StartingRoom() override = default;

	// 重写基类方法
	void Start(const std::shared_ptr<Character>& player) override;
	void Update() override;

	void LoadFromJSON() override;
};

#endif //STARTINGROOM_HPP
