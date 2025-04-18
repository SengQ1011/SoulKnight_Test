//
// Created by tjx20 on 4/17/2025.
//

#ifndef ENEMYAISTRATEGY_HPP
#define ENEMYAISTRATEGY_HPP

#include "StructType.hpp"

class Character;
//--------------------------------------------
// Strategy Interfaces
//--------------------------------------------
class IUtilityStrategy {
public:
    virtual ~IUtilityStrategy() = default;
    virtual void Update(const EnemyContext& ctx) = 0;
};

//--------------------------------------------
// Strategy Implementations
//--------------------------------------------
class SummonUtility : public IUtilityStrategy {
public:
	// 召喚類型
    void Update(const EnemyContext& ctx) override {

    }
};

class NoUtility : public IUtilityStrategy {
public:
    void Update(const EnemyContext& ctx) override {
        // Nothing
    }
};


#endif //ENEMYAISTRATEGY_HPP
