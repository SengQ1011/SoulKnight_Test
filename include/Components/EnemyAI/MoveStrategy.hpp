//
// Created by tjx20 on 4/17/2025.
//

#ifndef MOVESTRATEGY_HPP
#define MOVESTRATEGY_HPP

#include <random>
#include <cmath>
#include "Components/MovementComponent.hpp"
#include "Components/StateComponent.hpp"
#include "StructType.hpp"

//--------------------------------------------
// Strategy Interfaces
//--------------------------------------------
class IMoveStrategy {
public:
	virtual ~IMoveStrategy() = default;
	virtual void Update(const EnemyContext& ctx, float deltaTime) = 0;

	// 獲取範圍 [min, max] 內的隨機浮點數
	static float RandomFloatInRange(const float min, const float max) {
		std::random_device rd;  // 獲取隨機設備
		std::mt19937 gen(rd());  // 用隨機設備作為種子初始化生成器
		std::uniform_real_distribution<float> dis(min, max);  // 均勻分佈，範圍是 [min, max]

		return dis(gen);  // 生成亂數
	}
	// 生成一個單位圓內的隨機方向
	static glm::vec2 RandomDirectionInsideUnitCircle() {
		// 創建一個亂數產生器
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(0.0f, 2.0f * 3.14159265359f);  // 隨機生成角度，範圍 [0, 2π]

		// 生成一個隨機角度
		float angle = dis(gen);

		// 使用極座標轉換為笛卡爾座標，生成一個單位向量
		return glm::vec2(std::cos(angle), std::sin(angle)); // 歸一化單位圓上的隨機點
	}
};

//--------------------------------------------
// Strategy Implementations
//--------------------------------------------
class WanderMove final : public IMoveStrategy {
public:
	void Update(const EnemyContext& ctx, const float deltaTime) override;

	// TODO 暫時功能
	void ChasePlayerLogic(const EnemyContext& ctx, std::shared_ptr<nGameObject> target) const;

private:
	glm::vec2 m_wanderDirection = glm::vec2(0, 0);
	float m_restTimer = 0.0f;     // 休息時間計時器
	float m_wanderCooldown = 0;   // 移動時間計時器
	float m_detectionRange = 200.0f;
};

class ChaseMove final : public IMoveStrategy {
public:
	void Update(const EnemyContext& ctx, float deltaTime) override;
	void ChasePlayerLogic(const EnemyContext& ctx, std::shared_ptr<nGameObject> target) const;
};

class NoMove final : public IMoveStrategy {
public:
	// Do nothing
	void Update(const EnemyContext& ctx, float deltaTime) override {}
};

#endif //MOVESTRATEGY_HPP
