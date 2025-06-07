//
// Created by QuzzS on 2025/4/11.
//

#ifndef LOADER_HPP
#define LOADER_HPP

#include <json.hpp>

class Loader {
public:
	explicit Loader(std::string theme): m_Theme(std::move(theme)) {}
	~Loader() = default;
	nlohmann::ordered_json LoadLobbyObjectPosition() {return readJsonFile(JSON_DIR"/Lobby/ObjectPosition.json");};
	nlohmann::ordered_json LoadStartingRoomObjectPosition() {return readJsonFile(JSON_DIR"/"+ m_Theme+"/StartingRoom/ObjectPosition.json");}
	nlohmann::ordered_json LoadPortalRoomObjectPosition() {return readJsonFile(JSON_DIR"/"+ m_Theme+"/PortalRoom/ObjectPosition.json");}
	nlohmann::ordered_json LoadMonsterRoomObjectPosition() {return readJsonFile(JSON_DIR"/"+ m_Theme+"/MonsterRoom/2717ObjectPosition_1.json");}

	// 因爲Dungeon的Theme不同所以要變數
	nlohmann::ordered_json LoadObjectData(const std::string& ID) {return readJsonFile(JSON_DIR"/"+ m_Theme + "/ObjectData/" + ID + ".json");}

	nlohmann::ordered_json readJsonFile(const std::string& filePath);
protected:
	std::string m_Theme;
};

#endif //LOADER_HPP
