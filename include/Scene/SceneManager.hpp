//
// Created by QuzzS on 2025/3/6.
//

#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "Scene/Scene.hpp"

struct SaveData;
class SceneManager
{
public:
	// 單例模式 (Singleton Pattern)：取得唯一的SceneManager
	static SceneManager &GetInstance();

	// 禁止拷貝與賦值，確保只有一個實例
	SceneManager(const SceneManager &) = delete;
	SceneManager &operator=(const SceneManager &) = delete;

	std::weak_ptr<Scene> GetCurrentScene() { return m_CurrentScene; }

	// 場景初始化用功能
	void ChangeCurrentScene();

	// 新增：允許外部直接設定下一個場景
	void SetNextScene(Scene::SceneType nextScene);

	void Start();
	void Update() const; // 游戲循環 （場景切換 + 場景更新）
	void End(); // NOLINT(readability-convert-member-functions-to-static)

	void StartGame();
	void ForceInitializeNewGame(); // 重置關卡進度開始新遊戲
	void ResetGameProgress(); // 只重置 GameProgress，保留 GameData
	std::shared_ptr<SaveData> DownloadGameProgress() const;
	void UploadGameProgress(std::shared_ptr<SaveData> &updatedProgress);
	void receiveEnemyDeathEvent() const;

protected:
	static std::shared_ptr<Scene> CreateScene(Scene::SceneType type);

private:
	SceneManager() = default;

	Scene::SceneType m_NextSceneType = Scene::SceneType::Null;
	std::shared_ptr<Scene> m_CurrentScene = nullptr;
	std::shared_ptr<SaveData> m_Data = nullptr;
	// TODO: Dungeon預加載指標

	void InitializeNewGameData();
};


#endif // SCENEMANAGER_HPP
