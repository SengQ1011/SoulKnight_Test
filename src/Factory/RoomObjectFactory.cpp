//
// Created by QuzzS on 2025/3/27.
//

#include "Factory/RoomObjectFactory.hpp"

#include "Animation.hpp"
#include "ImagePoolManager.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "RoomObject/DestructibleBox.hpp"

#include "RoomObject/WallObject.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

// class可能是指定類型再用， 目前都是RoomObject
std::shared_ptr<nGameObject> RoomObjectFactory::createRoomObject(const std::string &_id, const std::string &_class)
{
	if (!m_ObjectDataFilePath.data())
	{
		LOG_DEBUG("RoomObjectFactory::createRoomObject 沒設置ObjectDataPath");
		return nullptr;
	}
	if (_id == "object_transferGate_0") LOG_DEBUG("YES1");
	// 是否是動畫
	bool isAnimated = false;
	nlohmann::json jsonData = m_Loader.lock()->LoadObjectData(_id);
	if (jsonData.contains("isAnimated"))
		isAnimated = jsonData["isAnimated"].get<bool>();

	std::shared_ptr<nGameObject> roomObject;

	// 根據 className 創建特定類型的對象
	if (!_class.empty())
	{
		if (_id == "object_transferGate_0") LOG_DEBUG("YES2");
		if (_class == "Wall" || _class == "WallObject")
		{
			roomObject = std::make_shared<WallObject>(_id);
		}
		else if (_class == "DestructibleBox")
		{
			roomObject = std::make_shared<DestructibleBox>(_id);
		}
		else
		{
			// 對於其他類型，使用原有邏輯
			if (isAnimated)
			{
				if (_id == "object_transferGate_0") LOG_DEBUG("YES3");
				std::vector<std::string> path = jsonData["path"].get<std::vector<std::string>>();
				for (auto &i : path)
					i = RESOURCE_DIR + i;
				std::shared_ptr<Animation> animation = std::make_shared<Animation>(path, true);
				animation->PlayAnimation(true);
				roomObject = animation;
			}
			else
			{
				if (_id == "object_transferGate_0") LOG_DEBUG("YES4");
				roomObject = std::make_shared<nGameObject>(_id);
			}
		}
	}
	else
	{
		// 沒有指定 className 時的默認行為
		if (isAnimated)
		{
			if (_id == "object_transferGate_0") LOG_DEBUG("YES5");
			std::vector<std::string> path = jsonData["path"].get<std::vector<std::string>>();
			for (auto &i : path)
				i = RESOURCE_DIR + i;
			std::shared_ptr<Animation> animation = std::make_shared<Animation>(path, true);
			animation->PlayAnimation(true);
			roomObject = animation;
		}
		else
		{
			roomObject = std::make_shared<nGameObject>(_id);
		}
	}

	// 設置Drawable（Animation 已經在構造函數中設置了）
	if (jsonData.contains("path") && !isAnimated)
	{
		roomObject->SetDrawable(
			ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR + jsonData.at("path").get<std::string>()));
	}
	else if (!jsonData.contains("path") && !isAnimated)
	{
		LOG_WARN("RoomObjectFactory::createRoomObject: No path for {}", _id);
	}

	// 設置ZIndexLayer
	if (jsonData.contains("ZIndex"))
	{
		const auto zIndexStr = jsonData.at("ZIndex").get<std::string>();
		roomObject->SetZIndexType(stringToZIndexType(zIndexStr));
	}
	else
	{
		LOG_WARN("RoomObjectFactory::createRoomObject: No ZIndex for {}", _id);
		roomObject->SetZIndexType(ZIndexType::CUSTOM); // 設置默認值
		roomObject->SetZIndex(100.0f);
	}

	// 設置posOffset
	if (jsonData.contains("posOffset"))
	{
		const auto data_posOffset = jsonData.at("posOffset");
		const auto posOffset = glm::vec2(data_posOffset[0].get<float>(), data_posOffset[1].get<float>());
		roomObject->SetPosOffset(posOffset);
	}

	// 設置Components
	if (!jsonData.contains("components"))
		return roomObject; // 沒有就跳過
	for (auto &component : jsonData.at("components"))
	{
		try
		{
			Factory::createComponent(roomObject, component);
		}
		catch (const std::exception &e)
		{
			LOG_ERROR("RoomObjectFactory::createRoomObject: {}", e.what());
		}
	}
	return roomObject;
}

// 專門用於地形創建的方法
std::shared_ptr<nGameObject> RoomObjectFactory::createWall(int row, int col, const glm::vec2 &worldPos)
{
	auto wall = std::static_pointer_cast<WallObject>(createRoomObject("w604", "Wall"));
	if (wall)
	{
		wall->SetWorldCoord(worldPos);
		wall->SetGridPosition({col, row});
		// 調整牆壁的位置偏移
		auto currentOffset = wall->GetPosOffset();
		wall->SetPosOffset(currentOffset + glm::vec2{0, 1.5f});
	}
	return wall;
}

std::shared_ptr<nGameObject> RoomObjectFactory::createFloor(const glm::vec2 &worldPos)
{
	auto floor = createRoomObject("f601", "Floor");
	if (floor)
	{
		floor->SetWorldCoord(worldPos);
	}
	return floor;
}

std::shared_ptr<nGameObject> RoomObjectFactory::createDoor(const glm::vec2 &worldPos)
{
	auto door = createRoomObject("object_door_0", "Door");
	if (door)
	{
		door->SetWorldCoord(worldPos);
	}
	return door;
}
