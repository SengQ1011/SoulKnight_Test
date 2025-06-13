//
// Created by QuzzS on 2025/4/29.
//

#ifndef ATTACKTRIGGERSTRATEGY_HPP
#define ATTACKTRIGGERSTRATEGY_HPP

#include "ITriggerStrategy.hpp"
enum class StatusEffect;

class AttackTriggerStrategy : public ITriggerStrategy
{
public:
	int m_Damage;
	StatusEffect m_elementalDamage;
	bool m_isCriticalHit;

	explicit AttackTriggerStrategy(const int damage, const StatusEffect &elementalDamage, const bool isCrit = false) :
		m_Damage(damage), m_elementalDamage(elementalDamage), m_isCriticalHit(isCrit)
	{
	}

	void OnTriggerEnter(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other) override;
};

#endif // ATTACKTRIGGERSTRATEGY_HPP
