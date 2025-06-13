//
// Created by tjx20 on 4/18/2025.
//

#ifndef ATTACK_HPP
#define ATTACK_HPP


struct AttackInfo;
struct EffectAttackInfo;
struct ProjectileInfo;
class EffectAttack;
#include "Override/nGameObject.hpp"

struct ChainAttackInfo
{
	bool enabled = false; // 是否啟用連鎖攻擊
	AttackType attackType; // 連鎖攻擊類型
	std::shared_ptr<AttackInfo> nextAttackInfo = nullptr; // 連鎖攻擊的攻擊資訊
};

struct AttackInfo
{
	virtual ~AttackInfo() = default; // 讓 AttackInfo 成為多型類型
	CharacterType type;
	Util::Transform attackTransform;
	glm::vec2 direction = glm::vec2(0.0f);
	float size = 0.0f;
	int damage = 0;
	StatusEffect elementalDamage = StatusEffect::NONE;
	bool isCriticalHit = false;

	ChainAttackInfo chainAttack;
};

class Attack : public nGameObject
{
public:
	explicit Attack(const AttackInfo &attack_info);
	~Attack() override = default;

	virtual void Init() = 0;
	virtual void UpdateObject(float deltaTime) = 0;

	std::string GetName() const override { return "Attack"; }

	//----Getter----
	[[nodiscard]] glm::vec2 GetWorldPosition() const { return m_Transform.translation; }
	[[nodiscard]] CharacterType GetAttackLayerType() const { return m_type; }
	[[nodiscard]] glm::vec2 GetAttackDirection() const { return m_direction; }
	[[nodiscard]] int GetDamage() const { return m_damage; }
	[[nodiscard]] StatusEffect GetStatusEffect() const { return m_elementalDamage; }
	[[nodiscard]] ChainAttackInfo GetChainAttackInfo() const { return m_chainAttack; }
	[[nodiscard]] bool ShouldRemove() const { return m_markRemove; }
	[[nodiscard]] bool WillDisappearOnHit() const { return m_disappearOnHit; }

	//----Setter----
	void SetDirection(const glm::vec2 &direction) { m_direction = direction; }
	void MarkForRemoval() { m_markRemove = true; }
	void SetDisappearOnHit(const bool disappear) { m_disappearOnHit = disappear; }
	void SetAttackCharacterType(const CharacterType type) { m_type = type; }

	void TriggerChainAttack() const;

protected:
	CharacterType m_type;
	glm::vec2 m_direction; // 方向
	bool m_markRemove = false; // 標記是否因碰撞需移除
	float m_size;
	int m_damage;
	bool m_disappearOnHit = true;
	bool m_isCriticalHit = false;

	StatusEffect m_elementalDamage;
	ChainAttackInfo m_chainAttack;
};

#endif // ATTACK_HPP
