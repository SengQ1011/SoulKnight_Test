//
// Created by QuzzS on 2025/3/4.
//
#include "Scene/Test_Scene_KC.hpp"

#include "Components/CollisionComponent.hpp"
#include "Components/FollowerComponent.hpp"

#include "Components/InteractableComponent.hpp"
#include "Cursor.hpp"
#include "Scene/SceneManager.hpp"
#include "pch.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"


void TestScene_KC::Start()
{
	LOG_DEBUG("Entering KC Test Scene");

	// 创建并初始化玩家
	CreatePlayer();

	// 创建并初始化大厅房间
	m_LobbyRoom = std::make_shared<LobbyRoom>();
	m_LobbyRoom->Start(m_Camera,m_Player);

	// 将玩家注册到碰撞管理器
	m_LobbyRoom->GetCollisionManager()->RegisterNGameObject(m_Player);
	// 将玩家添加到房间
	m_LobbyRoom->CharacterEnter(m_Player);

	//建立傳送門
	m_Portal->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/way_battle.png"));
	m_Portal->SetZIndexType(ZIndexType::OBJECTHIGH);
	m_Portal->SetWorldCoord(glm::vec2(0.0f, 128.0f));

	auto component = m_Portal->AddComponent<InteractableComponent>(ComponentType::INTERACTABLE);
	component->SetInteractionRadius(71.0f/2);
	component->SetInteractionCallback(
		[this](const std::shared_ptr<Character>& character, std::shared_ptr<nGameObject> owner)
		{
			isChange = true;
		});

	m_LobbyRoom->AddRoomObject(m_Portal);
	m_Camera->AddChild(m_Portal);
	m_Root->AddChild(m_Portal);

	// 设置相机
	SetupCamera();

	// 初始化场景管理器
	InitializeSceneManagers();
}

void TestScene_KC::Update()
{
	// Input处理
	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	inputManager->listenInput();

	if (Util::Input::IsKeyUp(Util::Keycode::F)) m_LobbyRoom->GetInteractionManager()->TryInteractWithClosest();

	m_Player->Update();

	// 更新房间
	m_LobbyRoom->Update();

	// 更新相机
	m_Camera->Update();

	// 更新场景根节点
	GetRoot().lock()->Update();
}

void TestScene_KC::CreatePlayer()
{
	// 使用 CharacterFactory 创建玩家
	m_Player = CharacterFactory::GetInstance().createPlayer(1);

	// 设置玩家的初始位置
	m_Player->SetWorldCoord(glm::vec2(0, 16*2)); // 初始位置为右两格，上两格

	// 获取碰撞组件并添加到场景和相机
	auto collision = m_Player->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (collision) {
		// 将碰撞盒添加到场景根节点和相机
		GetRoot().lock()->AddChild(collision->GetVisibleBox());
		m_Camera->AddChild(collision->GetVisibleBox());
	}

	// 将玩家添加到场景根节点和相机
	GetRoot().lock()->AddChild(m_Player);
	m_Camera->AddChild(m_Player);

}


void TestScene_KC::SetupCamera()
{
	m_Camera->SetFollowTarget(m_Player);
}

void TestScene_KC::InitializeSceneManagers()
{
	// 添加管理器到场景
	AddManager(ManagerTypes::INPUT, std::make_shared<InputManager>());
	AddManager(ManagerTypes::ROOMCOLLISION, m_LobbyRoom->GetCollisionManager());

	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	// 注册输入观察者
	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);
}

void TestScene_KC::Exit()
{
	LOG_DEBUG("KC Test Scene exited");
	// 退出场景时的清理工作
	if (m_LobbyRoom) {
		m_LobbyRoom->CharacterExit(std::dynamic_pointer_cast<Character>(m_Player));
	}
}

Scene::SceneType TestScene_KC::Change()
{
	if (isChange)
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}
