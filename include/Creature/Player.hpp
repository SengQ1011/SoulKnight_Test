//
// Created by QuzzS on 2025/3/4.
//

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Creature/Character.hpp"
#include "GameMechanism/Skill.hpp"
#include "GameMechanism/Talent.hpp"

class Player final: public Character {
public:
	// 構造函數：初始化玩家屬性
	Player(std::unordered_map<State, std::shared_ptr<Animation>> m_Animations, int maxHp, float speed, int aimRange, std::unique_ptr<CollisionBox> collisionBox, std::shared_ptr<Weapon> initialWeapon,
		   int maxArmor, int maxEnergy, double criticalRate, int handBladeDamage, std::shared_ptr<Skill> skill);

	void Start() override;
	void Update(float deltaTime) override;
	void recoverArmor();						// 自動回復護甲
	void useEnergy(int amount);					// 消耗能量

	void addTalent(const Talent& talent);		// 添加天賦
	void useSkill(Skill& skill);				// 施放技能
	void move(const glm::vec2 movement) override;		// 移動
	void AddWeapon(std::shared_ptr<Weapon> weapon) override; // 限制最多2把武器
	void switchWeapon();						// 切換武器

private:
	// 屬性
	int m_maxArmor;         // 護甲上限
	int m_currentArmor;     // 當前護甲值
	int m_maxEnergy;        // 能量上限
	int m_currentEnergy;    // 當前能量值
	double m_criticalRate;	// 暴擊率（0-1）
	int m_handBladeDamage;	// 手刀傷害
	std::shared_ptr<Skill> skill;			// 角色技能
	std::vector<Talent> talents;       // 天賦系統

};

#endif //PLAYER_HPP
