//
// Created by QuzzS on 2025/4/20.
//

#ifndef DUNGEONMAP_HPP
#define DUNGEONMAP_HPP

#include <memory>
#include <vector>

#include "DungeonRoom.hpp"
#include "glm/vec2.hpp"


enum class RoomType;
class Loader;
class RoomObjectFactory;
class Character;
class DungeonRoom;

struct DungeonMapSpaceInfo
{
	glm::vec2 tileSize = glm::vec2{16.0f};
	glm::vec2 roomRegion = glm::vec2{35.0f};
	glm::vec2 roomNum = glm::vec2{5};
	float m_MapHeight = roomNum.y * roomRegion.y * tileSize.y;
};

struct RoomInfo
{
	std::shared_ptr<DungeonRoom> room;
	RoomType m_RoomType = RoomType::EMPTY;
	std::array<bool, 4> m_Connections = {false, false, false, false};
};

constexpr std::array<Direction, 4> ALL_DIRECTIONS = {
	Direction::UP, Direction::RIGHT, Direction::DOWN, Direction::LEFT
};

class DungeonMap {
public:
	DungeonMap(const std::shared_ptr<RoomObjectFactory>& roomObjectFactory, const std::shared_ptr<Loader>& loader, const std::shared_ptr<Character>& player) :
		m_RoomObjectFactory(roomObjectFactory), m_Loader(loader), m_Player(player) {}
	void Start();
	void Update();

	// 房間生成
	bool GenerateMainPath();

	void SetPlayer(const std::shared_ptr<Character>& player) {m_Player = player;}

	float GetMapHeight() const {return m_SpaceInfo.m_MapHeight;}
	std::shared_ptr<DungeonRoom> GetCurrentRoom() {return m_CurrentRoom;}
	std::vector<std::weak_ptr<DungeonRoom>> GetNeighborRooms() const;

protected:
	std::weak_ptr<Character> m_Player;
	std::vector<std::shared_ptr<DungeonRoom>> m_Rooms; // << 改掉
	RoomInfo m_RoomInfo[5*5];
	std::shared_ptr<DungeonRoom> m_CurrentRoom;
	DungeonMapSpaceInfo m_SpaceInfo;

	std::weak_ptr<Loader> m_Loader;
	std::weak_ptr<RoomObjectFactory> m_RoomObjectFactory;

	void UpdateCurrentRoomIfNeeded();
	Direction GetOppositeDirection(Direction dir);
	glm::ivec2 Move(const glm::ivec2& pos, Direction dir);
	std::optional<Direction> GetRandomValidDirection(
		const glm::ivec2& currentPos,
		const std::set<Direction>& exclude = {}
		);
};

#endif //DUNGEONMAP_HPP
