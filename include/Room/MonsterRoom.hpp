//
// Created by QuzzS on 2025/4/23.
//

#ifndef MONSTERROOM_HPP
#define MONSTERROOM_HPP

#include "DungeonRoom.hpp"

class MonsterRoom : public DungeonRoom {
	explicit MonsterRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader>& loader, const std::shared_ptr<RoomObjectFactory>& room_object_factory)
		: DungeonRoom(worldCoord, loader, room_object_factory) {};
	explicit MonsterRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader>& loader, const std::shared_ptr<RoomObjectFactory>& room_object_factory, const glm::vec2& mapGridPos)
	: DungeonRoom(worldCoord, loader, room_object_factory,mapGridPos) {};
	~MonsterRoom() override = default;

	void Update() override;
};

#endif //MONSTERROOM_HPP
