//
// Created by QuzzS on 2025/3/4.
//

#ifndef LOBBY_SCENE_HPP
#define LOBBY_SCENE_HPP

#include "Attack/AttackManager.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Loader.hpp"
#include "Scene/Scene.hpp"
#include "Util/BGM.hpp"


class LobbyRoom;
class KeyPanel;

class LobbyScene : public Scene
{
public:
	LobbyScene() : Scene(SceneType::Lobby) {}
	~LobbyScene() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

	[[nodiscard]] std::shared_ptr<Character> GetPlayer() const { return m_Player; }

protected:
	std::shared_ptr<LobbyRoom> m_LobbyRoom;

	std::shared_ptr<Character> m_Player;
	std::shared_ptr<Character> m_NPC1;
	std::shared_ptr<Character> m_NPC2;
	std::vector<std::shared_ptr<Character>> m_Enemies;
	std::shared_ptr<Character> m_Enemy;

	float m_MapHeight = 0.0f;

	std::shared_ptr<RoomObjectFactory> m_RoomObjectFactory;

	std::shared_ptr<Loader> m_Loader = std::make_shared<Loader>("Lobby");
	std::shared_ptr<Util::BGM> m_BGM = std::make_shared<Util::BGM>(RESOURCE_DIR "/UI/bgm_openingLow.wav");

	std::string m_ThemeName = "Lobby"; // 工廠和房間加載JSON用的 TODO:可能叫SceneManager傳入

	void CreatePlayer();
	void CreateEnemy();
	std::shared_ptr<Character> CreateNPC(int id, glm::vec2 pos);
	void SetupCamera() const;
	void InitializeSceneManagers();
	void InitUIManager();
	void InitAudioManager();
};

#endif // LOBBY_SCENE_HPP
