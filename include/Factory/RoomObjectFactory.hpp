//
// Created by QuzzS on 2025/3/27.
//

#ifndef ROOMOBJECTFACTORY_HPP
#define ROOMOBJECTFACTORY_HPP
#include "Factory.hpp"
#include "RoomObject/RoomObject.hpp"

class RoomObjectFactory : public Factory{
public:
	RoomObjectFactory() = default;
	~RoomObjectFactory() override = default;
	// 根據配置文件創建物件
	std::shared_ptr<RoomObject> createRoomObject(const std::string& _id, const std::string& _class);

	// 批量創建物件 (適合房間初始化時使用)-->GPT
	std::vector<std::shared_ptr<RoomObject>> createObjectsFromJson(const nlohmann::json& objectsJson);

};

#endif //ROOMOBJECTFACTORY_HPP
