//
// Created by QuzzS on 2025/4/11.
//

#ifndef LOADER_HPP
#define LOADER_HPP

#include <json.hpp>

// 前向聲明
class RoomLayoutManager;

class Loader
{
public:
	explicit Loader(std::string theme);
	~Loader() = default;

	nlohmann::ordered_json LoadLobbyObjectPosition() { return readJsonFile(JSON_DIR "/Lobby/ObjectPosition.json"); };
	nlohmann::ordered_json LoadStartingRoomObjectPosition()
	{
		return readJsonFile(JSON_DIR "/" + m_Theme + "/StartingRoom/ObjectPosition.json");
	}
	nlohmann::ordered_json LoadPortalRoomObjectPosition()
	{
		return readJsonFile(JSON_DIR "/" + m_Theme + "/PortalRoom/ObjectPosition.json");
	}

	// 原有的固定方法（向後兼容）
	nlohmann::ordered_json LoadMonsterRoomObjectPosition()
	{
		return readJsonFile(JSON_DIR "/" + m_Theme + "/MonsterRoom/2717ObjectPosition_1.json");
	}

	// 新的隨機和指定佈局方法
	nlohmann::ordered_json LoadMonsterRoomObjectPosition_Random();
	nlohmann::ordered_json LoadMonsterRoomObjectPosition_Specific(const std::string &layoutName);

	// 測試用：取得所有可用佈局
	std::vector<std::string> GetAllMonsterRoomLayouts() const;

	// 因爲Dungeon的Theme不同所以要變數
	nlohmann::ordered_json LoadObjectData(const std::string &ID)
	{
		return readJsonFile(JSON_DIR "/" + m_Theme + "/ObjectData/" + ID + ".json");
	}

	nlohmann::ordered_json readJsonFile(const std::string &filePath);

	// 取得佈局管理器（用於測試介面）
	std::shared_ptr<RoomLayoutManager> GetLayoutManager() const { return m_LayoutManager; }

protected:
	std::string m_Theme;
	std::shared_ptr<RoomLayoutManager> m_LayoutManager;
};

#endif // LOADER_HPP
