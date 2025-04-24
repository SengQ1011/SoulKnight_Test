//
// Created by QuzzS on 2025/4/23.
//

#include "Room/MonsterRoom.hpp"
#include "Components/DoorComponent.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "Util/Keycode.hpp"
#include "Util/Input.hpp"

void MonsterRoom::Start(const std::shared_ptr<Character> &player)
{
	Room::Start(player);

	CreateGrid();
	m_CombatWave.totalWaves = 2;
}


void MonsterRoom::Update()
{
	Room::Update();

	if (Util::Input::IsKeyDown(Util::Keycode::B))
	{
		if (m_CombatWave.HasNextWave()) m_CombatWave.NextWave();
		else m_CombatWave.EndCombat();
	}
}

void MonsterRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadMonsterRoomObjectPosition();
	InitializeRoomObjects(jsonData);
}

void MonsterRoom::TryActivateByPlayer()
{
	switch (m_State)
	{
	case RoomState::UNEXPLORED:
		SetState(RoomState::COMBAT); // 內部觸發 OnStateChanged()
		break;
	case RoomState::COMBAT:
		if (m_CombatWave.IsFinished()) {
			SetState(RoomState::EXPLORED); // 進入清除狀態
		}
		break;
	default:
		break;
	}
}

void MonsterRoom::OnStateChanged()
{
	switch (m_State)
	{
	case RoomState::COMBAT:
		if (m_Doors.empty()) {
			LOG_ERROR("MonsterRoom::OnStateChanged: No door in the room");
			return;
		}
		m_CombatWave.StartCombat(m_CombatWave.totalWaves); // ⚠️ 放在外層，只呼叫一次
		for (const auto& door : m_Doors) {
			auto doorComp = door->GetComponent<DoorComponent>(ComponentType::DOOR);
			if (doorComp) doorComp->DoorClosed();
		}
		break;

	case RoomState::EXPLORED:
		for (const auto& door : m_Doors) {
			auto doorComp = door->GetComponent<DoorComponent>(ComponentType::DOOR);
			if (doorComp) doorComp->DoorOpened(); // 通常 EXPLORED 要開門
		}
		// TODO: 顯示寶箱或獎勵生成
		break;

	default:
		// 如果狀態本來就是 UNEXPLORED 或 ACTIVE，不是錯誤，這樣寫更保險
			LOG_DEBUG("MonsterRoom::OnStateChanged: No action needed for state {}", static_cast<int>(m_State));
		break;
	}
}

void MonsterRoom::CombatWaveInfo::StartCombat(int waveCount)
{
	totalWaves = waveCount;
	currentWave = 1;
	isInCombat = true;
}

bool MonsterRoom::CombatWaveInfo::IsCurrentWaveCleared() const
{
	// 實際判斷可以根據房間內敵人是否全滅
	return true; // 這裡先給個假的判斷
}

bool MonsterRoom::CombatWaveInfo::HasNextWave() const
{
	return currentWave < totalWaves;
}

void MonsterRoom::CombatWaveInfo::NextWave()
{
	if (HasNextWave()) {
		++currentWave;
	} else {
		EndCombat();
	}
}

void MonsterRoom::CombatWaveInfo::EndCombat()
{
	isInCombat = false;
	currentWave = 0;
	totalWaves = 0;
}

bool MonsterRoom::CombatWaveInfo::IsFinished() const
{
	return !isInCombat;
}








