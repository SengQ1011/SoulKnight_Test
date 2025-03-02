//
// Created by tjx20 on 3/2/2025.
//

#include "Player.hpp"

Player::Player(const std::string& ImagePath, int maxHp, double speed, int aimRange, double radius,
			   int maxArmor, int maxEnergy, double criticalRate, int handBladeDamage,
			   Weapon* primaryWeapon, Weapon* secondaryWeapon, const Skill& skill)
	: Character(ImagePath, maxHp, speed, aimRange, radius), maxArmor(maxArmor), maxEnergy(maxEnergy),
	  criticalRate(criticalRate), handBladeDamage(handBladeDamage), primaryWeapon(primaryWeapon), secondaryWeapon(secondaryWeapon),
	  currentArmor(maxArmor), currentEnergy(maxEnergy), skill(skill) {
	// 可以在這裡做一些初始化
}

void Player::attack(Character& target) {
	// 實現玩家的攻擊邏輯
}

void Player::recoverArmor() {
	// 恢復護甲的邏輯
}

void Player::useEnergy(int amount) {
	// 消耗能量的邏輯
}

void Player::switchWeapon() {
	// 切換武器的邏輯
}

void Player::changeWeapon() {
	// 更換武器的邏輯
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

