//
// Created by QuzzS on 2025/3/4.
//

#include <utility>

#include "Creature/Player.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"

Player::Player(const std::string& ImagePath, int maxHp, float speed, int aimRange, std::unique_ptr<CollisionBox> collisionBox , std::unique_ptr<Weapon>  initialWeapon,
			   int maxArmor, int maxEnergy, double criticalRate, int handBladeDamage, std::shared_ptr<Skill> skill)
	: Character(ImagePath, maxHp, speed, aimRange, std::move(collisionBox), std::move(initialWeapon)), m_maxArmor(maxArmor), m_currentArmor(maxArmor),
	m_maxEnergy(maxEnergy), m_currentEnergy(maxEnergy), m_criticalRate(criticalRate), m_handBladeDamage(handBladeDamage), skill(std::move(skill)) {}

void Player::Start()
{

}

void Player::Update() {

}

void Player::recoverArmor() {
	// 恢復護甲的邏輯
}

void Player::useEnergy(int amount) {
	// 消耗能量的邏輯
}

void Player::AddWeapon(std::unique_ptr<Weapon> newWeapon) {
	if (m_Weapons.size() >= 2) {
		// 武器數量已達上限，先移除當前武器
		RemoveWeapon(currentWeapon.get());
	}
	// 替換為新武器
	currentWeapon = std::move(newWeapon);
	m_Weapons.push_back(std::move(currentWeapon));  // 添加新武器
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

void Player::move(const glm::vec2 movement) {
	if ((movement.x < 0 && this->m_Transform.scale.x > 0) ||
		(movement.x > 0 && this->m_Transform.scale.x < 0)) {
		this->m_Transform.scale.x *= -1.0f;
	}
	this->m_WorldCoord += (movement * this->m_moveSpeed);
}

void Player::switchWeapon(){
	if (m_Weapons.size() == 2) {
		if (currentWeapon == m_Weapons[0]) {
			currentWeapon = std::move(m_Weapons[1]);
		} else {
			currentWeapon = std::move(m_Weapons[0]);
		}

		// 更新武器清單，確保 currentWeapon 更新後不會丟失
		m_Weapons.clear();
		m_Weapons.push_back(std::move(currentWeapon));
	}
	else {
		LOG_DEBUG("Not enough weapons to switch!");
	}
}


