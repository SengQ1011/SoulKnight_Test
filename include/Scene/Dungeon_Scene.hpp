//
// Created by QuzzS on 2025/3/4.
//

#ifndef DUNGEON_HPP
#define DUNGEON_HPP

#include "Scene/Scene.hpp"
#include "Util/SFX.hpp"
#include "Util/BGM.hpp"

class DungeonMap;

class DungeonScene final: public Scene
{
public:
	DungeonScene() = default;
	~DungeonScene() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

	static void GenerateStaticDungeon();
	static std::shared_ptr<DungeonScene> GetPreGenerated();
	static void ClearPreGenerated();
	std::shared_ptr<Character> GetPlayer() const { return m_Player; }

private:
	static std::shared_ptr<DungeonScene> s_PreGeneratedInstance;

	std::shared_ptr<Character> m_Player;
	float m_MapHeight;

	std::shared_ptr<DungeonMap> m_Map;
	std::shared_ptr<RoomObjectFactory> m_RoomObjectFactory;


	std::string m_ThemeName = "IcePlains";//工廠和房間加載JSON用的 TODO:可能叫SceneManager傳入
	std::shared_ptr<Loader> m_Loader;

	std::shared_ptr<Util::BGM> m_BGM = std::make_shared<Util::BGM>(RESOURCE_DIR"/UI/bgm_openingLow.wav");

	//test
	// std::vector<std::shared_ptr<nGameObject>> m_RoomObjectGroup;
	float m_timer = 0.0f;
	// std::shared_ptr<nGameObject> m_OnDeathText;
	std::shared_ptr<nGameObject> m_stageText;
	float m_textTimer = 2.0f;
	std::shared_ptr<Util::SFX> m_ClickSound = std::make_shared<Util::SFX>(RESOURCE_DIR"/plankton-augh.mp3");


	void CreatePlayer();
	void SetupCamera() const;
	void InitializeSceneManagers();

	// 以下為你原本地牢生成的函數
	void BuildDungeon(); // 房間生成與初始化邏輯

protected:
};

#endif //DUNGEON_HPP
