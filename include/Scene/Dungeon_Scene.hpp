//
// Created by QuzzS on 2025/3/4.
//

#ifndef DUNGEON_HPP
#define DUNGEON_HPP

#include "Scene/Scene.hpp"
#include "Util/BGM.hpp"
#include "Util/SFX.hpp"


class DungeonMap;
class MonsterRoomTestUI;

class DungeonScene final : public Scene
{
public:
	DungeonScene() : Scene(SceneType::Dungeon) {}
	~DungeonScene() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

	static void GenerateStaticDungeon();
	static std::shared_ptr<DungeonScene> GetPreGenerated();
	static void ClearPreGenerated();
	std::shared_ptr<Character> GetPlayer() const { return m_Player; }

	// 處理關卡完成的方法，供傳送門調用
	void OnStageCompleted();

private:
	static std::shared_ptr<DungeonScene> s_PreGeneratedInstance;

	std::shared_ptr<Character> m_Player;
	bool m_IsPlayerDeath = false;
	float m_MapHeight;

	std::shared_ptr<DungeonMap> m_Map;
	std::shared_ptr<RoomObjectFactory> m_RoomObjectFactory;


	std::string m_ThemeName = "IcePlains"; // 工廠和房間加載JSON用的 TODO:可能叫SceneManager傳入
	std::shared_ptr<Loader> m_Loader;

	// test
	//  std::vector<std::shared_ptr<nGameObject>> m_RoomObjectGroup;
	float m_timer = 0.0f;
	// std::shared_ptr<nGameObject> m_OnDeathText;
	std::shared_ptr<nGameObject> m_stageText;
	std::shared_ptr<nGameObject> m_stageIcon;
	float m_textTimer = 2.0f;

	// MonsterRoom 測試UI
	std::shared_ptr<MonsterRoomTestUI> m_MonsterRoomTestUI;
	float m_LayoutChangeCooldown = 0.0f; // 佈局更換冷卻時間

	void CreatePlayer();
	void SetupCamera() const;
	void InitializeSceneManagers();
	void InitUIManager();
	void InitAudioManager();
	void InitializeStageText();
	void InitializeStageIcon();
	void DrawStageDebugUI(); // 調試界面

	// 佈局更換相關方法
	void HandleLayoutChangeInput(); // 處理佈局更換輸入
	void ProcessLayoutChangeRequest(); // 處理佈局更換請求
	void ChangeCurrentRoomLayout(const std::string &layoutName); // 更換當前房間佈局
	void InitializeMonsterRoomTestUI(); // 初始化測試UI

	// 以下為你原本地牢生成的函數
	void BuildDungeon(); // 房間生成與初始化邏輯

protected:
};

#endif // DUNGEON_HPP
