//
// Created by tjx20 on 4/17/2025.
//

#ifndef MOVESTRATEGY_HPP
#define MOVESTRATEGY_HPP

#include <random>
#include <glm/vec2.hpp>
#include <memory>


struct CollisionEventInfo;
struct EnemyContext;
class nGameObject;

//--------------------------------------------
// Strategy Interfaces
//--------------------------------------------
class IMoveStrategy
{
public:
	virtual ~IMoveStrategy() = default;
	virtual void Update(const EnemyContext &ctx, float deltaTime) = 0;
	void CollisionAction(const CollisionEventInfo &info, const EnemyContext &ctx);

	// 獲取範圍 [min, max] 內的隨機浮點數
	static float RandomFloatInRange(const float min, const float max)
	{
		std::random_device rd; // 獲取隨機設備
		std::mt19937 gen(rd()); // 用隨機設備作為種子初始化生成器
		std::uniform_real_distribution<float> dis(min, max); // 均勻分佈，範圍是 [min, max]

		return dis(gen); // 生成亂數
	}
	// 生成一個單位圓內的隨機方向
	static glm::vec2 RandomDirectionInsideUnitCircle()
	{
		// 創建一個亂數產生器
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(0.0f, 2.0f * 3.14159265359f); // 隨機生成角度，範圍 [0, 2π]

		// 生成一個隨機角度
		float angle = dis(gen);

		// 使用極座標轉換為笛卡爾座標，生成一個單位向量
		return glm::vec2(std::cos(angle), std::sin(angle)); // 歸一化單位圓上的隨機點
	}

protected:
	float m_restTimer = 0.0f; // 休息時間計時器
	float m_moveTimer = 0; // 移動時間計時器
	float m_detectionRange = 250.0f;

	void changeToIdle(const EnemyContext &ctx);
};

//--------------------------------------------
// Strategy Implementations
//--------------------------------------------
class WanderMove final : public IMoveStrategy
{
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override;
};

class ChaseMove final : public IMoveStrategy
{
public:
	void Update(const EnemyContext &ctx, float deltaTime) override;
	void ChasePlayerLogic(const EnemyContext &ctx, std::shared_ptr<nGameObject> target) const;

private:
	const float m_meleeAttackDistance = 50.0f;
	const float m_gunAttackDistance = 200.0f;
};

class NoMove final : public IMoveStrategy
{
public:
	// Do nothing
	void Update(const EnemyContext &ctx, float deltaTime) override {}
};


#endif //MOVESTRATEGY_HPP
