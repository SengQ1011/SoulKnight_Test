//
// Created by tjx20 on 3/24/2025.
//

#ifndef AICOMPONENT_HPP
#define AICOMPONENT_HPP

#include "Components/Component.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Util/Time.hpp"
#include <random>
#include <cmath>


class AIComponent : public Component , public TrackingObserver{
public:
	explicit AIComponent(AIType type, int monsterPoint)
	: Component(ComponentType::AI), m_aiType(type), m_monsterPoint(monsterPoint) {}
	~AIComponent() override = default;

	void Update() override = 0;

	void OnPlayerPositionUpdate(std::weak_ptr<Character> player) override {
		//TODO
		m_Target = player;
	}
	void OnPlayerLost() override {
		m_Target.reset();
	}


protected:
	// 獲取範圍 [min, max] 內的隨機浮點數
	static float RandomFloatInRange(float min, float max) {
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

	AIType m_aiType;
	enemyState m_enemyState;
	int m_monsterPoint;
	std::weak_ptr<nGameObject> m_Target;			// enemy鎖定目標的位置
	float m_behaviorTimer = 0;
};


#endif //AICOMPONENT_HPP