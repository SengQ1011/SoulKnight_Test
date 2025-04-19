//
// Created by QuzzS on 2025/3/27.
//

#include "Factory/RoomObjectFactory.hpp"

#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

// class可能是指定類型再用， 目前都是RoomObject
std::shared_ptr<nGameObject> RoomObjectFactory::createRoomObject(const std::string& _id, const std::string& _class)
{
	std::shared_ptr<nGameObject> roomObject = std::make_shared<nGameObject>(_id);

	if (!m_ObjectDataFilePath.data())
	{
		LOG_DEBUG("RoomObjectFactory::createRoomObject 沒設置ObjectDataPath");
		return nullptr;
	}

	nlohmann::json jsonData = m_Loader.lock()->LoadObjectData(_id);

	// 設置Drawable
	if (jsonData.contains("path")) {
		roomObject->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR+jsonData.at("path").get<std::string>()));
	} else {
		LOG_WARN("RoomObjectFactory::createRoomObject: No path for {}", _id);
	}

	// 設置ZIndexLayer
	if (jsonData.contains("ZIndex")) {
		const auto zIndexStr = jsonData.at("ZIndex").get<std::string>();
		roomObject->SetZIndexType(stringToZIndexType(zIndexStr));
	} else {
		LOG_WARN("RoomObjectFactory::createRoomObject: No ZIndex for {}", _id);
		roomObject->SetZIndexType(ZIndexType::CUSTOM); // 設置默認值
		roomObject->SetZIndex(100.0f);
	}

	//設置Components
	if (!jsonData.contains("components")) return roomObject; // 沒有就跳過
	for (auto& component : jsonData.at("components"))
	{
		try { Factory::createComponent(roomObject, component); }
		catch (const std::exception& e) { LOG_ERROR("RoomObjectFactory::createRoomObject: {}", e.what()); }
	}
	return roomObject;
}
