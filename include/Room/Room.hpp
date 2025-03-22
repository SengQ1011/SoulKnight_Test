//
// Created by QuzzS on 2025/3/7.
//

#ifndef ROOM_HPP
#define ROOM_HPP

#include "RoomCollisionManager.hpp"
#include "pch.hpp"



class Room {
public:
	Room();
	~Room();

	void virtual Start() {};
	void virtual Update() {};

protected:
	glm::vec2 m_WorldCoord = glm::vec2(0, 0);
	glm::vec2 m_Size = glm::vec2(0, 0);
	glm::vec2 m_TileSize = glm::vec2(0, 0);
	std::vector<std::shared_ptr<nGameObject>> roomObject = {};
	std::shared_ptr<RoomCollisionManager> collisionManager = std::make_shared<RoomCollisionManager>();

};

#endif //ROOM_HPP
