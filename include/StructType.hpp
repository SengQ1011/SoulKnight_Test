//
// Created by tjx20 on 4/18/2025.
//

#ifndef STRUCTTYPE_HPP
#define STRUCTTYPE_HPP

#include <memory>

class MovementComponent;
class StateComponent;
class AttackComponent;
class AIComponent;
class Character;

#define LAZY_FIELD(type, name) \
std::shared_ptr<type> name; \
std::weak_ptr<type> name##Weak;
// lazy lock
struct EnemyContext {
	LAZY_FIELD(Character, enemy)
	LAZY_FIELD(MovementComponent, moveComp)
	LAZY_FIELD(StateComponent, stateComp)
	LAZY_FIELD(AttackComponent, attackComp)

	void EnsureValid() {
		if (!enemy) enemy = enemyWeak.lock();
		if (!moveComp) moveComp = moveCompWeak.lock();
		if (!stateComp) stateComp = stateCompWeak.lock();
		if (!attackComp) attackComp = attackCompWeak.lock();
	}

	std::shared_ptr<AIComponent> GetAIComp() const;
};

#endif //STRUCTTYPE_HPP
