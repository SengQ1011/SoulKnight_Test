//
// Created by Assistant on 2025/1/20.
//

#ifndef IDESTRUCTIONEFFECT_HPP
#define IDESTRUCTIONEFFECT_HPP

#include <glm/glm.hpp>
#include <string>
#include "EnumTypes.hpp"

/**
 * @brief 破壞效果策略介面
 * 定義所有破壞效果的統一介面，使用策略模式實現不同的破壞效果
 */
class IDestructionEffect
{
public:
	virtual ~IDestructionEffect() = default;

	/**
	 * @brief 執行破壞效果
	 * @param position 效果觸發位置
	 * @param attackerType 攻擊者類型（玩家或敵人）
	 */
	virtual void Execute(const glm::vec2 &position, CharacterType attackerType) = 0;

	/**
	 * @brief 獲取效果名稱
	 * @return 效果名稱字符串
	 */
	virtual std::string GetEffectName() const = 0;
};

#endif // IDESTRUCTIONEFFECT_HPP
