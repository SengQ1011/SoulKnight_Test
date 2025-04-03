//
// Created by QuzzS on 2025/3/21.
//

//RoomFactory.hpp

#ifndef ROOMFACTORY_HPP
#define ROOMFACTORY_HPP

#include "Factory.hpp"

class RoomFactory : public Factory {
public:
	RoomFactory() = default;
	~RoomFactory() override = default;

	void SetJSON(const std::string& fileName) {m_JSONName = fileName;}

	std::shared_ptr<RoomFactory> createRoom();

protected:
	std::string m_JSONName;

//public:
// 	explicit RoomFactory(std::string configFile = "rooms.json")
// 			: m_configFile(std::move(configFile)) {}
//
// 	// 創建特定類型房間
// 	std::shared_ptr<Room> createRoom(const std::string& roomId);
//
// 	// 隨機創建房間 (適合地牢生成)
// 	std::shared_ptr<Room> createRandomRoom(RoomType type);
//
// private:
// 	std::string m_configFile;
// 	RoomObjectFactory m_objectFactory; // 組合使用
};

#endif //ROOMFACTORY_HPP
