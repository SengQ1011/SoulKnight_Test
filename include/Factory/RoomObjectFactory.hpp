//
// Created by QuzzS on 2025/3/27.
//

#ifndef ROOMOBJECTFACTORY_HPP
#define ROOMOBJECTFACTORY_HPP
#include "Factory.hpp"
#include "RoomObject/RoomObject.hpp"

class RoomObjectFactory : public Factory{
public:
	RoomObjectFactory() =  default;
	~RoomObjectFactory() override = default;
	// 根據角色配置文件創建角色
	std::shared_ptr<RoomObject> createRoomObject(const std::string& _id, const std::string& _class);
private:
};

#endif //ROOMOBJECTFACTORY_HPP
