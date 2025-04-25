//
// Created by QuzzS on 2025/3/27.
//

#include "Factory/RoomObjectFactory.hpp"

#include "Animation.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "RoomObject/ObstacleObject.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

// class可能是指定類型再用， 目前都是RoomObject
std::shared_ptr<nGameObject> RoomObjectFactory::createRoomObject(const std::string& _id, const std::string& _class)
{
	if (!m_ObjectDataFilePath.data())
	{
		LOG_DEBUG("RoomObjectFactory::createRoomObject 沒設置ObjectDataPath");
		return nullptr;
	}

	//是否是動畫
	bool isAnimated = false;
	nlohmann::json jsonData = m_Loader.lock()->LoadObjectData(_id);
	if (jsonData.contains("isAnimated")) isAnimated = jsonData["isAnimated"].get<bool>();

	std::shared_ptr<nGameObject> roomObject;
	// 設置Drawable
	if (jsonData.contains("path")) {
		if (isAnimated)
		{
			std::vector<std::string> path = jsonData["path"].get<std::vector<std::string>>();
			for (auto & i : path) i = RESOURCE_DIR + i;
			std::shared_ptr<Animation> animation = std::make_shared<Animation>(path,true);
			animation->PlayAnimation(true);
			roomObject = animation;
			LOG_DEBUG("SEE {} {}",std::dynamic_pointer_cast<Animation>(roomObject)->IsLooping(),std::dynamic_pointer_cast<Animation>(roomObject)->IsPlaying());
		}
		else
		{
			roomObject = std::make_shared<nGameObject>(_id);
			roomObject->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR+jsonData.at("path").get<std::string>()));
		}
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

	// 設置posOffset
	if (jsonData.contains("posOffset")) {
		const auto data_posOffset = jsonData.at("posOffset");
		const auto posOffset = glm::vec2(data_posOffset[0].get<float>(), data_posOffset[1].get<float>());
		roomObject->SetPosOffset(posOffset);
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
