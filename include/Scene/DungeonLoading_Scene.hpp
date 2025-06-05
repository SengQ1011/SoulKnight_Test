//
// Created by QuzzS on 2025/3/4.
//

#ifndef DUNGEONLOADING_HPP
#define DUNGEONLOADING_HPP

#include "Scene/Scene.hpp"

#include <future>
namespace Util
{
	class Text;
}
class DungeonLoadingScene : public Scene
{
public:
	DungeonLoadingScene() : Scene(SceneType::DungeonLoad) {}
	~DungeonLoadingScene() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

private:
	// std::future<void> m_GenerationTask; // 背景生成任務
	bool m_DungeonReady = false;        // 是否完成生成
	std::shared_ptr<nGameObject> m_Text ;
};

#endif //DUNGEONLOADING_HPP
