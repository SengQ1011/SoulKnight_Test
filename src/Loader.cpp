//
// Created by QuzzS on 2025/4/11.
//

#include "Loader.hpp"
#include <fstream>
#include "Room/RoomLayoutManager.hpp"
#include "Util/Logger.hpp"


Loader::Loader(std::string theme) : m_Theme(std::move(theme))
{
	// 初始化佈局管理器
	m_LayoutManager = std::make_shared<RoomLayoutManager>(m_Theme);
}

nlohmann::ordered_json Loader::LoadMonsterRoomObjectPosition_Random()
{
	std::string layoutName = m_LayoutManager->GetRandomLayout("MonsterRoom");
	std::string filePath = JSON_DIR "/" + m_Theme + "/MonsterRoom/" + layoutName + ".json";
	return readJsonFile(filePath);
}

nlohmann::ordered_json Loader::LoadMonsterRoomObjectPosition_Specific(const std::string &layoutName)
{
	std::string selectedLayout = m_LayoutManager->GetSpecificLayout(layoutName, "MonsterRoom");
	std::string filePath = JSON_DIR "/" + m_Theme + "/MonsterRoom/" + selectedLayout + ".json";
	return readJsonFile(filePath);
}

std::vector<std::string> Loader::GetAllMonsterRoomLayouts() const
{
	return m_LayoutManager->GetAllLayoutNames("MonsterRoom");
}

nlohmann::ordered_json Loader::readJsonFile(const std::string &filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		LOG_DEBUG("Error: Unable to open file: {}", filePath);
		return nlohmann::json(); // 如果文件打開失敗，返回空的 JSON 物件
	}

	nlohmann::json jsonData;
	file >> jsonData;
	return jsonData;
}
