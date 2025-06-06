//
// Created by QuzzS on 2025/4/25.
//

#ifndef PORTALROOM_HPP
#define PORTALROOM_HPP

#include "Room/DungeonRoom.hpp"

class PortalRoom final : public DungeonRoom
{
public:
	explicit PortalRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader> &loader,
						const std::shared_ptr<RoomObjectFactory> &room_object_factory,
						const glm::vec2 &mapGridPos = glm::vec2(0), const RoomType roomType = RoomType::PORTAL) :
		DungeonRoom(worldCoord, loader, room_object_factory, mapGridPos, roomType){};
	~PortalRoom() override = default;

	// 重写基类方法
	void Start(const std::shared_ptr<Character> &player) override;
	void Update() override;

	void LoadFromJSON() override;

private:
	void CreatePortal();
};

#endif // PORTALROOM_HPP
