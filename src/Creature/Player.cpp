//
// Created by QuzzS on 2025/3/4.
//

#include <utility>

#include "Creature/Player.hpp"

#include "Components/AnimationComponent.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

Player::Player(int maxHp, float speed, int aimRange, std::unique_ptr<CollisionBox> collisionBox , std::shared_ptr<Weapon>  initialWeapon,
			   int maxArmor, int maxEnergy, double criticalRate, int handBladeDamage, std::shared_ptr<Skill> skill)
	: Character(maxHp, speed, aimRange, std::move(collisionBox), initialWeapon), m_maxArmor(maxArmor), m_currentArmor(maxArmor),
	m_maxEnergy(maxEnergy), m_currentEnergy(maxEnergy), m_criticalRate(criticalRate), m_handBladeDamage(handBladeDamage), skill(std::move(skill)) {}

void Player::Start() {
	if (m_currentWeapon) {
		// m_currentWeapon->SetPivot(this->m_WorldCoord);
		m_currentWeapon->SetZIndex(15);
		LOG_DEBUG("have initWeapon");
	}
	else {
		LOG_DEBUG("Error: Don't have initWeapon!!!");
	}
}

void Player::Update(float deltaTime) {
	UpdateComponents(deltaTime);
	if (m_currentWeapon) {
		m_currentWeapon->Update();
		//m_currentWeapon->m_WorldCoord = this->m_WorldCoord;  // 更新武器位置为玩家位置
	}

	// 更新碰撞箱位置
	if (m_collisionBox) {
		m_collisionBox->SetCoordinate(this->m_WorldCoord);  // 更新碰撞箱位置为玩家位置
	}
}


void Player::recoverArmor() {
	// 恢復護甲的邏輯
}

void Player::useEnergy(int amount) {
	// 消耗能量的邏輯
}

void Player::AddWeapon(std::shared_ptr<Weapon> newWeapon) {
	if (m_Weapons.size() >= 2) {
		// 武器數量已達上限，先移除當前武器
		RemoveWeapon(m_currentWeapon.get());
	}
	// 替換為新武器
	m_currentWeapon = std::move(newWeapon);
	m_Weapons.push_back(std::move(m_currentWeapon));  // 添加新武器
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
	//m_state = State::MOVING;

	auto m_currentAnimation = this->GetComponent<AnimationComponent>(ComponentType::ANIMATION)->GetCurrentAnimation();
	if ((movement.x < 0 && m_currentAnimation->m_Transform.scale.x > 0) ||
		(movement.x > 0 && m_currentAnimation->m_Transform.scale.x < 0)) {
		m_currentAnimation->m_Transform.scale.x *= -1.0f;
		this->m_currentWeapon->m_Transform.scale.x *= -1.0f;
	}
	this->m_WorldCoord += (movement * this->m_moveSpeed);
}

void Player::switchWeapon(){
	if (m_Weapons.size() == 2) {
		if (m_currentWeapon == m_Weapons[0]) {
			m_currentWeapon = std::move(m_Weapons[1]);
		} else {
			m_currentWeapon = std::move(m_Weapons[0]);
		}

		// 更新武器清單，確保 currentWeapon 更新後不會丟失
		m_Weapons.clear();
		m_Weapons.push_back(std::move(m_currentWeapon));
	}
	else {
		LOG_DEBUG("Not enough weapons to switch!");
	}
}