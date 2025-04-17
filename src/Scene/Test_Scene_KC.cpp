//
// Created by QuzzS on 2025/3/4.
//
// ReSharper disable All
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
	m_Loader = std::make_shared<Loader>(m_ThemeName);

	// 创建并初始化玩家
	CreatePlayer();

	// 设置相机
	m_MapHeight = 480.0f ; //大廳場景的地圖高度 480.0f
	SetupCamera();

	//設置工廠
	m_RoomObjectFactory = std::make_shared<RoomObjectFactory>(m_Loader);

	// 创建并初始化大厅房间
	m_DungeonRoom = std::make_shared<DungeonRoom>(m_Loader,m_RoomObjectFactory);
	m_DungeonRoom->Start(m_Player);

	m_RoomObjectGroup = m_DungeonRoom->GetRoomObjects();

	// 将玩家注册到碰撞管理器
	m_DungeonRoom->GetCollisionManager()->RegisterNGameObject(m_Player);
	// 将玩家添加到房间
	m_DungeonRoom->CharacterEnter(m_Player);

	//test
	CreateVisibleGrid();

	// 初始化场景管理器
	InitializeSceneManagers();
}

void TestScene_KC::Update()
{
	// Input处理
	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	inputManager->Update();

	m_Player->Update();

	if (Util::Input::IsKeyUp(Util::Keycode::O))
	{
		for (const auto& row: m_Grid)
		{
			for (const auto& elem: row)
			{
				elem->SetVisible(true ^ elem->IsVisible());
			}
		}
	}

	// 更新房间
	m_DungeonRoom->Update();

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

void TestScene_KC::SetupCamera() const
{
	m_Camera->SetMapSize(m_MapHeight);
	m_Camera->SetFollowTarget(m_Player);
}

void TestScene_KC::InitializeSceneManagers()
{
	// 添加管理器到场景
	AddManager(ManagerTypes::INPUT, std::make_shared<InputManager>());
	AddManager(ManagerTypes::ROOMCOLLISION, m_DungeonRoom->GetCollisionManager());

	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	// 注册输入观察者
	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);
}

void TestScene_KC::Exit()
{
	LOG_DEBUG("KC Test Scene exited");
	// 退出场景时的清理工作
	if (m_DungeonRoom) {
		m_DungeonRoom->CharacterExit(std::dynamic_pointer_cast<Character>(m_Player));
	}
}

Scene::SceneType TestScene_KC::Change()
{
	if (IsChange())
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}

void TestScene_KC::CreateVisibleGrid()
{
	const float gridSize = 35.0f;
	const float tileSize = 16.0f;
	auto startPos = glm::vec2(gridSize * tileSize/2) - glm::vec2(tileSize / 2);
	startPos *= glm::vec2(-1,1);

	for (const auto& elem: m_RoomObjectGroup)
	{
		auto collisionComp = elem->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		if (!collisionComp) continue;

		Rect bound = collisionComp->GetBounds();


		glm::ivec2 startGrid = glm::vec2( std::floor((bound.left() - startPos.x) / tileSize), std::ceil((startPos.y - bound.top())/ tileSize) );
		glm::ivec2 endGrid = glm::vec2( std::ceil((bound.right() - startPos.x) / tileSize), std::floor((startPos.y - bound.bottom()) / tileSize) );

		// glm::ivec2 startGrid = glm::vec2( std::floor(bound.left() / tileSize), std::ceil(bound.top() / tileSize) );
		// glm::ivec2 endGrid = glm::vec2( std::ceil(bound.right() / tileSize), std::floor(bound.bottom() / tileSize) );
		LOG_DEBUG("Start Grid Start {} {}", startGrid, endGrid);
		for (int row = startGrid.y; row <= endGrid.y; row++)
		{
			for (int col = startGrid.x; col <= endGrid.x; col++)
			{
				glm::vec2 posGridCell = startPos + glm::vec2(static_cast<float>(col) * tileSize, -static_cast<float>(row) * tileSize);
				glm::vec2 sizeGridCell(tileSize);
				Rect gridCell(posGridCell, sizeGridCell);

				float intersect = IntersectionArea(bound, gridCell);
				LOG_DEBUG("Intersect {} ", intersect);
				float cellArea = tileSize * tileSize;

				if (intersect >= 0.5f * cellArea)
				{
					m_Mark[row][col] = 1;
				}
			}
		}
	}

	for (int row = 0; row < 35; row++)
	{
		for (int col = 0; col < 35; col++)
		{
			std::cout << m_Mark[row][col] << " ";
			if (m_Mark[row][col] == 0)
			{
				auto bound = m_RoomObjectFactory->createRoomObject("bound","nGameObject");
				glm::vec2 pos = startPos + glm::vec2(static_cast<float>(col) * tileSize, -static_cast<float>(row) * tileSize);
				bound->SetWorldCoord(pos);
				bound->SetZIndex(20);
				m_Camera->AddChild(bound);
				m_Root->AddChild(bound);
				m_Grid[row][col] = bound;
			}
			if (m_Mark[row][col] == 1)
			{
				auto bound = std::make_shared<nGameObject>();
				bound->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/BlueCollider.png"));
				glm::vec2 pos = startPos + glm::vec2(static_cast<float>(col) * tileSize, -static_cast<float>(row) * tileSize);
				bound->SetInitialScale(glm::vec2(16));
				bound->SetInitialScaleSet(true);
				bound->SetWorldCoord(pos);
				bound->SetZIndexType(CUSTOM);
				bound->SetZIndex(100);
				m_Camera->AddChild(bound);
				m_Root->AddChild(bound);
				m_Grid[row][col] = bound;
			}
		}
		std::cout << std::endl;
	}
}

float TestScene_KC::IntersectionArea(const Rect& a, const Rect& b) {
	LOG_DEBUG("a top:{} bottom:{} right:{} left:{}",
					a.top(), a.bottom(), a.right(), a.left());
	LOG_DEBUG("b top:{} bottom:{} right:{} left:{}",
					b.top(), b.bottom(), b.right(), b.left());
	std::cout <<std::endl;
	float left   = std::max(a.left(),   b.left());
	float right  = std::min(a.right(),  b.right());
	float top    = std::max(a.top(),    b.top());
	float bottom = std::min(a.bottom(), b.bottom());

	if (right <= left || top <= bottom)
		return 0.0f; // 無交集

	return (right - left) * (top - bottom);
}

