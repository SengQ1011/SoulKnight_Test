//
// Created by tjx20 on 4/19/2025.
//

#include "Attack/Attack.hpp"

 Attack::Attack(const AttackInfo& attack_info)
	 :m_type(attack_info.type), m_direction(attack_info.direction), m_size(attack_info.size), m_damage(attack_info.damage)
{
	this->m_Transform = attack_info.attackTransform;
}
