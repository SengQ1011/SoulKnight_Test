//
// Created by QuzzS on 2025/3/27.
//

#ifndef ROOMOBJECTFACTORY_HPP
#define ROOMOBJECTFACTORY_HPP
#include "Factory.hpp"
#include "RoomObject/RoomObject.hpp"

/// @brief:建造RoomObject前必須設置ScenePath(從Room取得)
class RoomObjectFactory : public Factory{
public:
	RoomObjectFactory() = default;
	~RoomObjectFactory() override = default;
	// 根據配置文件創建物件
	std::shared_ptr<RoomObject> createRoomObject(const std::string& _id, const std::string& _class);

	[[nodiscard]] std::string GetScenePath() const {return m_ScenePath;}
	void SetScenePath(const std::string &path) {m_ScenePath = path+"/ObjectData/";}

	std::string m_ScenePath; /// @param:"Scene(Theme)/ObjectData/"作爲_id的前綴

};

#endif //ROOMOBJECTFACTORY_HPP
