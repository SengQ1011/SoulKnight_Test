//
// Created by tjx20 on 4/17/2025.
//

#ifndef ATTACKSTRATEGY_HPP
#define ATTACKSTRATEGY_HPP

class Character;

//--------------------------------------------
// Strategy Interfaces
//--------------------------------------------
class IAttackStrategy {
public:
	virtual ~IAttackStrategy() = default;
	virtual void TryAttack(Character& owner) = 0;
};

//--------------------------------------------
// Strategy Implementations
//--------------------------------------------
class MeleeAttack final : public IAttackStrategy {
public:
	void TryAttack(Character& owner) override {

	}
};

class GunAttack final : public IAttackStrategy {
public:
	void TryAttack(Character& owner) override {

	}
};

class CollisionAttack final : public IAttackStrategy {
public:
	void TryAttack(Character& owner) override {

	}
};

class NoAttack final : public IAttackStrategy {
public:
	// No attack
	void TryAttack(Character& owner) override {}
};
#endif //ATTACKSTRATEGY_HPP
