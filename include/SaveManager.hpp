//
// Created by tjx20 on 5/21/2025.
//

#ifndef SAVEMANAGER_HPP
#define SAVEMANAGER_HPP

#pragma once
#include "Weapon/Weapon.hpp"
#include "json.hpp"
#include "Util/Time.hpp"

// 游戲過程玩家資訊
struct PlayerData {
    int playerID;					// 玩家的id
    int currentHp;					// 玩家目前的血量
    int currentEnergy;				// 玩家目前的能量值
	int money = 0 ;					// 游戲内玩家的錢
	std::vector<int> weaponID;		// 玩家所擁有的武器ID
	std::vector<int> talentID;		// 玩家所擁有的天賦ID
};

// 游戲進度
struct GameProgress {
	// int currentTheme;				// 目前的主題
    int currentChapter = 1;				// 目前的章節(1~3)==》對應 StageTheme
	int currentStage = 1;				// 目前的關卡數(1~5)
	Util::ms_t dungeonStartTime = 0;	// 開始時間（結算的時候計算）
	Util::ms_t cumulativeTime = 0;		// 纍積時間
	int killCount = 0;
	PlayerData playerData;				// 玩家資料
};

// 武器圖鑒
struct weaponAtlas {
	std::unordered_map<BaseWeaponInfo, bool> weaponData;
};

// 游戲資料
struct GameData {
	int gameMoney = 0;
	// weaponAtlas allWeapon;		// 武器圖鑒
};

// PlayerData
inline void to_json(nlohmann::json& j, const PlayerData& p) {
	j = nlohmann::json{
		        {"playerID", p.playerID},
				{"currentHp", p.currentHp},
				{"currentEnergy", p.currentEnergy},
				{"money", p.money},
				{"weaponID", p.weaponID},
				{"talentID", p.talentID}
	};
}

inline void from_json(const nlohmann::json& j, PlayerData& p) {
	j.at("playerID").get_to(p.playerID);
	j.at("currentHp").get_to(p.currentHp);
	j.at("currentEnergy").get_to(p.currentEnergy);
	j.at("money").get_to(p.money);
	j.at("weaponID").get_to(p.weaponID);
	j.at("talentID").get_to(p.talentID);
}

// GameProgress
inline void to_json(nlohmann::json& j, const GameProgress& g) {
	j = nlohmann::json{
		        {"currentChapter", g.currentChapter},
				{"currentStage", g.currentStage},
				{"dungeonStartTime", g.dungeonStartTime},
				{"cumulativeTime", g.cumulativeTime},
				{"killCount", g.killCount},
				{"playerData", g.playerData}
	};
}

inline void from_json(const nlohmann::json& j, GameProgress& g) {
	j.at("currentChapter").get_to(g.currentChapter);
	j.at("currentStage").get_to(g.currentStage);
	j.at("dungeonStartTime").get_to(g.dungeonStartTime);
	j.at("cumulativeTime").get_to(g.cumulativeTime);
	j.at("killCount").get_to(g.killCount);
	j.at("playerData").get_to(g.playerData);
}

// // BaseWeaponInfo
// inline void to_json(nlohmann::json& j, const BaseWeaponInfo& w) {
// 	j = nlohmann::json{{"imagePath", w.imagePath}, {"name", w.name}};
// }
//
// inline void from_json(const nlohmann::json& j, BaseWeaponInfo& w) {
// 	j.at("imagePath").get_to(w.imagePath);
// 	j.at("name").get_to(w.name);
// }

// GameData
inline void to_json(nlohmann::json& j, const GameData& d) {
	j = nlohmann::json{
		        {"gameMoney", d.gameMoney}
				//{"allWeapon", d.allWeapon}
	};
}

inline void from_json(const nlohmann::json& j, GameData& d) {
	j.at("gameMoney").get_to(d.gameMoney);
	//j.at("allWeapon").get_to(d.allWeapon);
}

struct SaveData {
    std::string saveName;
    std::string saveTime;
    bool isInGameProgress;
    GameProgress gameProgress;
    GameData gameData;

	// 儲存成 JSON
	[[nodiscard]] nlohmann::json toJson() const {
		return {
	            {"saveName", saveName},
				{"saveTime", saveTime},
				{"isInGameProgress", isInGameProgress},
				{"gameProgress", gameProgress},
				{"gameData", gameData}
		};
	}

	// 從 JSON 載入
	void fromJson(const nlohmann::json& j) {
		j.at("saveName").get_to(saveName);
		j.at("saveTime").get_to(saveTime);
		j.at("isInGameProgress").get_to(isInGameProgress);
		j.at("gameProgress").get_to(gameProgress);
		j.at("gameData").get_to(gameData);
	}
};

class SaveManager {
public:
	// 获取單例實例
	static SaveManager& GetInstance() {
		static SaveManager instance;
		return instance;
	}

	//====Getter====
	[[nodiscard]] std::shared_ptr<SaveData> GetSaveData() const { return saveSlot; }		// 获取存档信息
	[[nodiscard]] bool HasSaveData() const;												// 检查是否有存档

	//====Setter====
    bool SaveGame(const std::shared_ptr<SaveData>&  saveData);				// 保存游戲
    bool DeleteSave();														// 刪除存檔


private:
	std::string saveDirectory;
	std::shared_ptr<SaveData> saveSlot;

	// 單例模式：私有構造函數
	explicit SaveManager(const std::string &saveDir = "../saves/");

	// 禁用拷貝構造和賦值操作
	SaveManager(const SaveManager&) = delete;
	SaveManager& operator=(const SaveManager&) = delete;

    void CreateSaveDirectory() const;
    void LoadSaveData();
    static std::string GetCurrentTimeString();
};

#endif //SAVEMANAGER_HPP