//
// Created by QuzzS on 2025/3/4.
//
#include "Creature/Character.hpp"
#include "Util/Image.hpp"

#include "Creature/Character.hpp"
#include "Util/Image.hpp"

Character::Character(std::string name, CharacterType type)
	: m_name(name),m_type(type) {
	ResetPosition();
}
