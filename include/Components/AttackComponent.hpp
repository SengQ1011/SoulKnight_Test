//
// Created by tjx20 on 3/26/2025.
//

//AttackComponent.hpp

#ifndef ATTACKCOMPONENT_HPP
#define ATTACKCOMPONENT_HPP

#include "Components/Component.hpp"
#include "Observer.hpp"
#include "Util/Logger.hpp"

class Weapon;
class AttackComponent final : public Component, public TrackingObserver
{
public:
	explicit AttackComponent(const std::shared_ptr<Weapon> &initWeapon, const float criticalRate,
							 const int handBladeDamage, const int collisionDamage);
	~AttackComponent() override = default;

	void Init() override;
	void Update() override;

	//----Getter----
	[[nodiscard]] float GetAimRange() const { return m_aimRange; }
	[[nodiscard]] int GetMaxWeapon() const { return m_maxWeapon; }
	[[nodiscard]] std::shared_ptr<Weapon> GetCurrentWeapon() const { return m_currentWeapon; }
	[[nodiscard]] std::shared_ptr<Weapon> GetSecondWeapon() const { return m_secondWeapon; }
	[[nodiscard]] std::vector<std::shared_ptr<Weapon>> GetAllWeapons() const { return m_Weapons; }
	[[nodiscard]] bool HasTarget() const { return !m_Target.expired(); }
	[[nodiscard]] std::weak_ptr<nGameObject> GetTarget() const { return m_Target.lock(); }
	[[nodiscard]] int GetNumRebound() const { return m_numRebound; }
	[[nodiscard]] bool GetReflectBullet() const { return m_ReflectBullets; }
	[[nodiscard]] int GetCollisionDamage() const { return m_collisionDamage; }

	//----Setter----
	void SetMaxWeapon(const int num) { m_maxWeapon = num; }
	void AddWeapon(const std::shared_ptr<Weapon>& weapon);
	void RemoveAllWeapon() { m_Weapons.clear(); }
	void RemoveWeapon();
	void switchWeapon();
	void SetTarget(const std::shared_ptr<nGameObject> &target) { m_Target = target; }
	void SetNumRebound(const int num) { m_numRebound = num; }
	void SetReflectBullet(const bool enable) { m_ReflectBullets = enable; }
	void SetDualWield(const bool enable); // 雙武器
	void SetSecondWeapon(const std::shared_ptr<Weapon> &weapon)
	{
		if (!weapon)
			LOG_ERROR("weapon=nullptr");
		m_secondWeapon = weapon;
	}

	int calculateDamage();
	void TryAttack(); // 使用當前武器攻擊敵人
	void OnTargetPositionUpdate(std::weak_ptr<Character> enemy) override;
	void OnLostTarget() override;

	void HandleEvent(const EventInfo &eventInfo) override;
	std::vector<EventType> SubscribedEventTypes() const override;

private:
	float m_aimRange;					// 自動瞄準範圍
	float m_criticalRate;				// 攻擊暴擊率(計算傷害公式=(角色+武器)暴擊率 * 武器傷害)
	int m_maxWeapon;					// 最多武器數量
	int m_handBladeDamage;				// player:手刀傷害
	int m_collisionDamage;				// enemy:專用於Collision模式的傷害值
	int m_numRebound = 0;				// 天賦：子彈反彈
	bool m_ReflectBullets = false;		// 天賦：近戰反彈
	bool m_dualWield = false;			// 是否雙持武器
	const float m_switchCooldown = 1.0f;	// 冷卻時間（秒）
	float m_switchTimeCounter;				// 上次切換時間
	std::shared_ptr<Weapon> m_currentWeapon; // 目前裝備的武器
	std::shared_ptr<Weapon> m_secondWeapon; // 技能：雙持武器
	std::vector<std::shared_ptr<Weapon>> m_Weapons; // 每個角色都有武器
	std::weak_ptr<nGameObject> m_Target; // 此物件會跟隨目標旋轉
};


#endif //ATTACKCOMPONENT_HPP
