//
// Created by QuzzS on 2025/3/27.
//

#ifndef ROOMOBJECTFACTORY_HPP
#define ROOMOBJECTFACTORY_HPP
#include "Factory.hpp"
#include "Loader.hpp"
#include "RoomObject/RoomObject.hpp"

/// @brief:建造RoomObject前必須設置ScenePath(從Room取得)
class RoomObjectFactory : public Factory{
public:
	explicit RoomObjectFactory(const std::shared_ptr<Loader>& loader) : m_Loader(loader) {}
	~RoomObjectFactory() override = default;

	// 根據配置文件創建物件
	std::shared_ptr<RoomObject> createRoomObject(const std::string& _id, const std::string& _class);
	//	TODO: std::shared_ptr<RoomObject> createWall(const std::string& _id, const std::string& _class);

	[[nodiscard]] std::string GetObjectDataFilePath() const {return m_ObjectDataFilePath;}
	void SetObjectDataFilePath(const std::string &prePath) {m_ObjectDataFilePath = prePath+"ObjectData/";}

	// TODO:測試std::string m_ObjectDataFilePath; /// @param:"Scene(Theme)/ObjectData/"作爲_id的前綴
	std::string m_ObjectDataFilePath = JSON_DIR"/Lobby/ObjectData"; /// @param:"Scene(Theme)/ObjectData/"作爲_id的前綴
	std::weak_ptr<Loader> m_Loader;

};

#endif //ROOMOBJECTFACTORY_HPP
