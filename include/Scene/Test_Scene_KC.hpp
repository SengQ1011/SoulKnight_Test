//
// Created by QuzzS on 2025/3/4.
//

#ifndef TEST_SCENE_HPP
#define TEST_SCENE_HPP

#include "Attack/AttackManager.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Loader.hpp"
#include "Room/DungeonRoom.hpp"
#include "Scene/Scene.hpp"
#include "Util/BGM.hpp"

class DungeonMap;
class LobbyRoom;
class nGameObject;

class TestScene_KC final : public Scene
{
public:
	TestScene_KC() : Scene(SceneType::Test_KC) {}
	~TestScene_KC() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

private:
	// 私有方法，按功能划分场景初始化流程
	void CreatePlayer();
	void CreateStageText();
	void SetupCamera() const;
	void InitializeSceneManagers();
	void InitUIManager();
	void InitAudioManager();

protected:
	std::shared_ptr<LobbyRoom> m_LobbyRoom;

	std::shared_ptr<Character> m_Player;

	float m_MapHeight = 0.0f;

	std::shared_ptr<RoomObjectFactory> m_RoomObjectFactory;

	std::shared_ptr<Loader> m_Loader = std::make_shared<Loader>("Lobby");
	std::shared_ptr<Util::BGM> m_BGM = std::make_shared<Util::BGM>(RESOURCE_DIR "/UI/bgm_openingLow.wav");

	std::string m_ThemeName = "Lobby"; // 工廠和房間加載JSON用的 TODO:可能叫SceneManager傳入

	// 關卡文字
	std::shared_ptr<nGameObject> m_stageText;
	float m_stageTextTimer = 0.0f;

	// test
	std::vector<std::shared_ptr<nGameObject>> m_RoomObjectGroup;
};

#endif // TEST_SCENE_HPP
