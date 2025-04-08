//
// Created by QuzzS on 2025/3/27.
//

#include "Factory/RoomObjectFactory.hpp"

#include "Components/AnimationComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/FollowerComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/StateComponent.hpp"

// class可能是指定類型再用， 目前都是RoomObject
std::shared_ptr<RoomObject> RoomObjectFactory::createRoomObject(const std::string& _id, const std::string& _class)
{
	std::shared_ptr<RoomObject> roomObject = std::make_shared<RoomObject>();

	// 讀取 JSON 資料
	nlohmann::json origin = readJsonFile("LobbyObjectData.json");

	// 檢查ID是否存在
	if (!origin.contains(_id)) {
		LOG_ERROR("RoomObjectFactory::createRoomObject: No such ID in JSON: {}", _id);
		return roomObject;
	}
	//TODO:ID系統

	auto data = origin[_id];

	// 設置Drawable
	if (data.contains("path")) {
		roomObject->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR+data.at("path").get<std::string>()));
	} else {
		LOG_WARN("RoomObjectFactory::createRoomObject: No path for {}", _id);
	}

	// 設置ZIndexLayer
	if (data.contains("ZIndex")) {
		const auto zIndexStr = data.at("ZIndex").get<std::string>();
		roomObject->SetZIndexType(stringToZIndexType(zIndexStr));
	} else {
		LOG_WARN("RoomObjectFactory::createRoomObject: No ZIndex for {}", _id);
		roomObject->SetZIndexType(ZIndexType::CUSTOM); // 設置默認值
		roomObject->SetZIndex(100.0f);
	}

	//設置Components
	if (!data.contains("components")) return roomObject; // 沒有就跳過
	for (auto& component : data.at("components"))
	{
		try { Factory::createComponent(roomObject, component); }
		catch (const std::exception& e) { LOG_ERROR("RoomObjectFactory::createRoomObject: {}", e.what()); }
	}
	return roomObject;
}
