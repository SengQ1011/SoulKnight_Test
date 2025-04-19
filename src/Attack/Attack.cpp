//
// Created by tjx20 on 4/19/2025.
//

#include "Attack/Attack.hpp"

 Attack::Attack(const CharacterType type, const Util::Transform &attackTransform, const glm::vec2 direction, float size, int damage)
	 :m_type(type), m_direction(direction), m_size(size), m_damage(damage)
{
	this->m_Transform = attackTransform;
}
