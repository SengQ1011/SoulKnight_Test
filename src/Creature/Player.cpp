//
// Created by QuzzS on 2025/3/4.
//

#include "Creature/Player.hpp"
#include "Util/Time.hpp"

Player::Player(const std::string& ImagePath, int maxHp, float speed, int aimRange, CollisionBox* radius , Weapon* initialWeapon,
			   int maxArmor, int maxEnergy, double criticalRate, int handBladeDamage, Skill* skill)
	: Character(ImagePath, maxHp, speed, aimRange, radius, initialWeapon), m_maxArmor(maxArmor), m_currentArmor(maxArmor),
	m_maxEnergy(maxEnergy), m_currentEnergy(maxEnergy), m_criticalRate(criticalRate), m_handBladeDamage(handBladeDamage), skill(skill) {}

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

void Player::move(const glm::vec2 movement) {
	if (movement.x < 0 && this->m_Transform.scale.x > 0) {this->m_Transform.scale.x *= -1.0f;}
	// 向右移動（D）
	else if (movement.x > 0 && this->m_Transform.scale.x < 0) {this->m_Transform.scale.x *= -1.0f;}
	this->m_WorldCoord += (movement * this->m_moveSpeed);
}

