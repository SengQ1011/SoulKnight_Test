//
// Created by QuzzS on 2025/3/4.
//

#include <utility>

#include "Creature/Player.hpp"

#include "Components/AnimationComponent.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

Player::Player(int maxHp, float speed, std::unique_ptr<CollisionBox> collisionBox,
			   int maxArmor, int maxEnergy, std::shared_ptr<Skill> skill)
	: Character(maxHp, speed, std::move(collisionBox)),  // 初始化基类Character的构造函数
	  m_maxArmor(maxArmor), m_currentArmor(maxArmor),
	  m_maxEnergy(maxEnergy), m_currentEnergy(maxEnergy), m_skill(std::move(skill)) {}  // 初始化Player类的成员变量


void Player::Start() {

}




void Player::recoverArmor() {
	// 恢復護甲的邏輯
}

void Player::useEnergy(int amount) {
	// 消耗能量的邏輯
}



void Player::addTalent(const Talent& talent) {
	talents.push_back(talent);
}

void Player::useSkill(Skill& skill) {
	if (skill.remainingCooldown <= 0.0) {
		skill.Execute();  // 執行技能
	} else {
		LOG_DEBUG("Skill is on cooldown!");
	}
}