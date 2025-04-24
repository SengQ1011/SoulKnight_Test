//
// Created by QuzzS on 2025/4/23.
//

#ifndef MONSTERROOM_HPP
#define MONSTERROOM_HPP

#include "DungeonRoom.hpp"


class MonsterRoom final : public DungeonRoom {
public:
	explicit MonsterRoom(
		const glm::vec2 worldCoord,
		const std::shared_ptr<Loader>& loader,
		const std::shared_ptr<RoomObjectFactory>& room_object_factory,
		const glm::vec2& mapGridPos = glm::vec2(0),
		const RoomType roomType = RoomType::MONSTER)
	: DungeonRoom(worldCoord, loader, room_object_factory,mapGridPos,roomType) {};
	~MonsterRoom() override = default;

	void Start(const std::shared_ptr<Character>& player) override;
	void Update() override;

	void LoadFromJSON() override;

	void TryActivateByPlayer() override;
	void OnStateChanged() override;

private:
	struct CombatWaveInfo {
		int currentWave = 0;
		int totalWaves = 0;
		bool isInCombat = false;

		void StartCombat(int waveCount);
		bool IsCurrentWaveCleared() const;
		bool HasNextWave() const;
		void NextWave();
		void EndCombat();
		bool IsFinished() const;
	};

	CombatWaveInfo m_CombatWave;
};

#endif //MONSTERROOM_HPP
