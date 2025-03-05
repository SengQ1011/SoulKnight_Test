//
// Created by QuzzS on 2025/3/4.
//

#include "Creature/Player.hpp"

Player::Player(const std::string& ImagePath, int maxHp, int currentHp, float speed, int aimRange, double radius,
			   int maxArmor, int maxEnergy, double criticalRate, int handBladeDamage, Skill& skill, Weapon* initialWeapon)
	: Character(ImagePath, maxHp, currentHp, speed, aimRange, radius, initialWeapon), m_maxArmor(maxArmor), m_maxEnergy(maxEnergy),
	  m_criticalRate(criticalRate), m_handBladeDamage(handBladeDamage), skill(skill), m_currentArmor(maxArmor), m_currentEnergy(maxEnergy) {}

void Player::attack() {
	if (currentWeapon) {
		//currentWeapon->use();
	}
}

void Player::recoverArmor() {
	// 恢復護甲的邏輯
}

void Player::useEnergy(int amount) {
	// 消耗能量的邏輯
}

void Player::AddWeapon(Weapon* weapon) {
	if (m_Weapons.size() < 2) { // 限制最多兩把武器
		return Character::AddWeapon(weapon);
	}
	else{
		// 更換當前的武器
	}
}


void Player::addTalent(const Talent& talent) {
	talents.push_back(talent);
}

void Player::useSkill(Skill& skill) {
	if (skill.remainingCooldown <= 0.0) {
		skill.Execute();  // 執行技能
	} else {
		std::cout << "Skill is on cooldown!" << std::endl;
	}
}

void Player::move() {

}

