#ifndef LOBBYROOM_HPP
#define LOBBYROOM_HPP

#include "Room/Room.hpp"
#include <vector>
#include <memory>
#include "glm/glm.hpp"

class nGameObject;

class LobbyRoom : public Room
{
public:
	explicit LobbyRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader> &loader,
					   const std::shared_ptr<RoomObjectFactory> &room_object_factory) :
		Room(worldCoord, loader, room_object_factory)
	{
	}
	~LobbyRoom() override = default;

	// 重写基类方法
	void Start(const std::shared_ptr<Character> &player) override;
	void Update() override;

	void AddWallCollider(const std::shared_ptr<nGameObject> &collider);
	std::vector<std::shared_ptr<nGameObject>> GetWallColliders() { return m_WallColliders; };
	void CreateEgg(const glm::vec2 &position, const std::vector<std::shared_ptr<nGameObject>>& dropItems = {}); 