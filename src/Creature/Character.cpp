//
// Created by QuzzS on 2025/3/4.
//
#include "Creature/Character.hpp"
#include "Util/Image.hpp"

#include "Creature/Character.hpp"
#include "Util/Image.hpp"

Character::Character(std::unordered_map<State, std::shared_ptr<Animation>> Animations, int maxHp, float speed, int aimRange, std::unique_ptr<CollisionBox> collisionBox, std::shared_ptr<Weapon> initialWeapon)
	: m_Animations(Animations), m_maxHp(maxHp),m_currentHp(maxHp), m_moveSpeed(speed), m_aimRange(aimRange), m_collisionBox(std::move(collisionBox)), m_currentWeapon(initialWeapon) {
	SetState(State::STANDING);
	ResetPosition();
	AddWeapon(initialWeapon);

	// 初始化動畫
	for (auto& pair : m_Animations) {
		if (pair.second) {
			pair.second->PlayAnimation(false);
			LOG_DEBUG("Animation for state " + std::to_string(static_cast<int>(pair.first)) + " initialized");
		}
	}

	// 確保當前動畫正確
	SetAnimation(m_state);

	if (m_currentAnimation) {
		this->AddChild(m_currentAnimation);  // 確保動畫加入角色
	}
	if (m_currentWeapon) {
		this->AddChild(m_currentWeapon);  // 確保武器加入角色
	}
	LOG_DEBUG("Character has been initialized");
}

void Character::Update(float deltaTime) {
	// 切換動畫
	if (m_state != m_previousState) {
		SetAnimation(m_state);
		m_previousState = m_state;
	}
}

void Character::SetAnimation(State state) {
	auto it = m_Animations.find(state);
	// 有找到相關的動畫
	if (it != m_Animations.end()) {
		// 移除舊的動畫
		if (m_currentAnimation) {
			m_currentAnimation->PlayAnimation(false);
			m_currentAnimation->SetVisible(false);
			this->RemoveChild(m_currentAnimation);
		}

		// 設定新動畫
		m_currentAnimation = it->second;
		m_currentAnimation->SetVisible(true);
		m_currentAnimation->PlayAnimation(true);

		// 新動畫加入 `Character`
		this->AddChild(m_currentAnimation);

		LOG_DEBUG("Switched animation to state " + std::to_string(static_cast<int>(state)));
	}
}

void Character::SetState(State newState) {
	m_state = newState;
}

void Character::attack() {
	m_state = State::ATTACK;
	if (m_currentWeapon) {
		m_currentWeapon->attack();
	}
}

void Character::takeDamage(int dmg) {
	m_currentHp -= dmg;
	if (this->isDead()) {
		m_currentHp = 0;
		m_state = State::DEAD;
	}
}

void Character::RemoveWeapon(Weapon* weapon) {
	auto it = std::find_if(m_Weapons.begin(), m_Weapons.end(),
		[weapon](std::shared_ptr<Weapon> w) { return w.get() == weapon; });

	if (it != m_Weapons.end()) {
		m_Weapons.erase(it);
	}
}

void Character::AddWeapon(std::shared_ptr<Weapon> newWeapon) {
	if (m_Weapons.empty()) {
		m_Weapons.push_back(newWeapon);
	}
}

bool Character::CheckCollides(const std::shared_ptr<Character> &other) const {
	return false;
}