//
// Created by QuzzS on 2025/3/4.
//

#include "Cursor.hpp"
#include "Scene/Test_Scene_JX.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"


void TestScene_JX::Start()
{
	LOG_DEBUG("Entering JX Test Scene");

	m_Background->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/Lobby/LobbyFloor.png"));
	m_Background->SetZIndex(1);

	//設置貓咪圖片
	m_Enemy->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/pet00icon.png"));
	m_Enemy->SetZIndex(10);

	m_Player->SetZIndex(1);
	m_Player->m_WorldCoord = {16*2,16*2}; //騎士初始位置為右兩格，上兩格
	m_Player->SetPivot(glm::vec2(0.5f, 0.5f)); // 設定為中心點

	m_Weapon->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/weapons_19.png"));
	m_Weapon->SetZIndex(15);
	m_Weapon->m_WorldCoord = m_Player->m_WorldCoord;

	// m_Beacon.SetReferenceObjectCoord(std::make_shared<glm::vec2>(Cursor::GetCursorWorldCoord()));

	//加入m_Root大家庭，才可以被渲染到熒幕上
	m_Root.AddChild(m_Background);
	m_Root.AddChild(m_Player);
	m_Root.AddChild(m_Enemy);
	m_Root.AddChild(m_Weapon);
	//加入了Camera大家庭，Camera移動會被影響，沒加就不會被影響
	//例如UI，不加入就可以固定在熒幕上
	m_Camera.AddRelativePivotChild(m_Background);
	m_Camera.AddRelativePivotChild(m_Player);
	m_Camera.AddRelativePivotChild(m_Enemy);
	m_Camera.AddRelativePivotChild(m_Weapon);
}

void TestScene_JX::Input()
{
	//目前還沒開發，專門處理玩家輸入
	//但是要把數據傳入Update需要將data儲存，在Update呼叫
	//需要討論各場景玩家輸入事件會有什麽data需要存
}

void TestScene_JX::Update()
{
	//LOG_DEBUG("Test Scene is running...");
	Cursor::SetWindowOriginWorldCoord(m_Camera.GetCameraWorldCoord().translation); //實時更新Cursor的世界坐標
	m_Weapon->m_WorldCoord = m_Player->m_WorldCoord; //實時更新武器的世界坐標，讓武器跟著敵人
	m_Camera.Update(); //更新Camera大家庭成員的渲染坐標

	// Input：位移量
	glm::vec2 movement(0.0f, 0.0f);
	if (Util::Input::IsKeyPressed(Util::Keycode::W))movement.y += 1.0f;
	if (Util::Input::IsKeyPressed(Util::Keycode::S)) movement.y -= 1.0f;
	if (Util::Input::IsKeyPressed(Util::Keycode::A)) movement.x -= 1.0f;
	if (Util::Input::IsKeyPressed(Util::Keycode::D)) movement.x += 1.0f;

	//Camera Zoom In=I /Out=K
	if (Util::Input::IsKeyPressed(Util::Keycode::I)) {m_Camera.ZoomCamera(1);}
	if (Util::Input::IsKeyPressed(Util::Keycode::K)) {m_Camera.ZoomCamera(-1);}


	if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB))
	{
		glm::vec2 cursor = Cursor::GetCursorWorldCoord(m_Camera.GetCameraWorldCoord().scale.x);
		LOG_DEBUG("Cursor coord:{}", cursor);
	}

	//TODO:待優化 目前匀速
	if (movement != glm::vec2(0.0f)) //檢查非零向量 才能進行向量標準化 length(speed) != 0模長非零，因爲normalize = speed / length(speed)
	{
		const float ratio = 0.2f;
		const glm::vec2 deltaDisplacement = normalize(movement) * ratio * Util::Time::GetDeltaTimeMs(); //normalize為防止斜向走速度是根號2
		m_Player->move(deltaDisplacement);
		m_Weapon->m_Transform.scale = m_Player->m_Transform.scale;
		m_Camera.MoveCamera(deltaDisplacement);
	}
	m_Root.Update();
}

void TestScene_JX::Exit()
{
	LOG_DEBUG("JX Test Scene exited");
}

Scene::SceneType TestScene_JX::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		//回傳需要切換的下個場景
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	//沒有要切換就一直回傳Null
	return Scene::SceneType::Null;
}

