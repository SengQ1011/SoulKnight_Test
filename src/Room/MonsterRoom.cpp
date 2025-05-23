//
// Created by QuzzS on 2025/4/23.
//

#include "Room/MonsterRoom.hpp"
#include "Components/DoorComponent.hpp"
#include "Creature/Character.hpp"
#include "Factory/CharacterFactory.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "Tool/Tool.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include <random>

void MonsterRoom::Start(const std::shared_ptr<Character> &player)
{
	Room::Start(player);

	CreateGrid();
	m_CombatWave.totalWaves = 1;
	SpawnEnemiesInRoom();
}


void MonsterRoom::Update()
{
	Room::Update();

	if (m_CombatWave.IsCurrentWaveCleared())
	{
		if (m_CombatWave.HasNextWave()) m_CombatWave.NextWave();
		else m_CombatWave.EndCombat();
	}
	// if (m_CombatWave.isInCombat) for (const auto& enemy: m_CombatWave.enemies) enemy->Update();
	// TODO: 回合 SetActive是一次性
	// if (m_CombatWave.isInCombat) for (const auto& enemy: m_CombatWave.enemies) enemy->SetActive(true);
}

void MonsterRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadMonsterRoomObjectPosition();
	InitializeRoomObjects(jsonData);
}

std::shared_ptr<Character> MonsterRoom::SpawnEnemy(int id, glm::vec2 pos)
{
	auto enemy = CharacterFactory::GetInstance().createEnemy(id);
	enemy->m_WorldCoord = pos;
	RegisterObjectToSceneAndManager(enemy);
	return enemy;
}

void MonsterRoom::SpawnEnemiesInRoom()
{
	const glm::vec2& tileSize = m_RoomSpaceInfo.m_TileSize;
	const glm::vec2& roomSize = m_RoomSpaceInfo.m_RoomSize;
	const glm::vec2& region = m_RoomSpaceInfo.m_RoomRegion;
	const glm::vec2& roomCoord = m_RoomSpaceInfo.m_WorldCoord;

	const int regionWidth = static_cast<int>(region.x);
	const int regionHeight = static_cast<int>(region.y);
	const int roomWidth = static_cast<int>(roomSize.x);
	const int roomHeight = static_cast<int>(roomSize.y);

	// RoomSize 的左上角在 region 區域中的起始格子位置
	const int startX = std::ceil((regionWidth + 1 - roomWidth) / 2);
	const int startY = std::ceil((regionHeight + 1 - roomHeight) / 2);
	const int endX = startX + roomWidth - 2;
	const int endY = startY + roomHeight - 2;

	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int> xDist(startX, endX - 1);
	std::uniform_int_distribution<int> yDist(startY, endY - 1);

	const int maxEnemies = 3;
	int retries = 100;

	m_CombatWave.enemies.clear();
	m_CombatWave.aliveEnemyCount = 0;

	while (m_CombatWave.enemies.size() < maxEnemies && retries-- > 0)
	{
		int col = xDist(rng);
		int row = yDist(rng);
		if (m_Mark[row][col] != 0) continue;

		glm::ivec2 grid = {col, row};
		const glm::vec2 worldPos = Tool::RoomGridToWorld(grid, tileSize, roomCoord, region);
		std::uniform_int_distribution<int> ID(1, 12);

		if (const auto enemy = SpawnEnemy(ID(rng), worldPos)) {
			enemy->SetActive(false);
			m_CombatWave.enemies.push_back(enemy);
			++m_CombatWave.aliveEnemyCount;
			m_Mark[row][col] = 1;
		}
	}
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
		m_CombatWave.StartCombat(m_CombatWave.totalWaves);
		for (const auto& door : m_Doors) {
			auto doorComp = door->GetComponent<DoorComponent>(ComponentType::DOOR);
			if (doorComp) doorComp->DoorClosed();
			for (const auto& enemy: m_CombatWave.enemies) enemy->SetActive(true);
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

void MonsterRoom::OnEnemyDied()
{
	if (m_CombatWave.isInCombat && m_CombatWave.aliveEnemyCount > 0)
		--m_CombatWave.aliveEnemyCount;
}


void MonsterRoom::CombatWaveInfo::StartCombat(int waveCount)
{
	totalWaves = waveCount;
	currentWave = 1;
	isInCombat = true;

}

bool MonsterRoom::CombatWaveInfo::IsCurrentWaveCleared() const
{
	if (aliveEnemyCount == 0) return true;
	return false; // 這裡先給個假的判斷
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








