//
// Created by tjx20 on 5/19/2025.
//

#ifndef RANDOMUTIL_HPP
#define RANDOMUTIL_HPP

#include <random>
#include <glm/vec2.hpp>

class RandomUtil {
public:
	// 獲取範圍 [min, max] 內的隨機數
	static int RandomIntInRange(int min, int max) {
		// 用 C++11 random
		static std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<int> dist(min, max);
		return dist(rng);
	}

	static float RandomFloatInRange(const float min, const float max)
	{
		static std::mt19937 gen(std::random_device{}()); // 用隨機設備作為種子初始化生成器
		std::uniform_real_distribution<float> dis(min, max);// 均勻分佈，範圍是 [min, max]
		return dis(gen); // 生成亂數
	}

	// 生成一個單位圓內的隨機方向
	static glm::vec2 RandomDirectionInsideUnitCircle()
	{
		static std::mt19937 gen(std::random_device{}());
		std::uniform_real_distribution<float> dis(0.0f, 2.0f * 3.14159265359f);// 隨機生成角度，範圍 [0, 2π]
		// 生成一個隨機角度
		float angle = dis(gen);
		// 使用極座標轉換為笛卡爾座標，生成一個單位向量
		return glm::vec2(std::cos(angle), std::sin(angle));// 歸一化單位圓上的隨機點
	}
};


#endif //RANDOMUTIL_HPP
