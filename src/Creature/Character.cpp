//
// Created by QuzzS on 2025/3/4.
//

#include "Creature/Character.hpp"

Character::Character(const std::string &name, CharacterType type)
	: m_name(name),m_type(type) {
	ResetPosition();
	this->SetPivot(glm::vec2{0.5f,0.5f});
}
