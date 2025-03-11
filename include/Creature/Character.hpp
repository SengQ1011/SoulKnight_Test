//
// Created by QuzzS on 2025/3/4.
//

#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <string>
#include <vector>
#include "GameMechanism/StatusEffect.hpp"
#include "Override/nGameObject.hpp"
#include "Weapon/Weapon.hpp"
#include "Tool/CollisionBox.hpp"
#include <memory>

enum class State
{
	STANDING,
	MOVING,
	ATTACK,
	DEAD
};

class Character : public nGameObject {
public:
	// 建構子+指定角色的圖片(explicit：防止單參數構造函數進行隱式轉換)
	explicit Character(const std::string& ImagePath, int maxHp,float moveSpeed, int aimRange, std::unique_ptr<CollisionBox> collisionRadius, std::unique_ptr<Weapon> initialWeapon);
	~Character() override = default;

	// delete function--> 禁止 Character 被複製或移動,確保遊戲角色的唯一性
	Character(const Character&) = delete;
	Character(Character&&) = delete;
	Character& operator=(const Character&) = delete;
	Character& operator=(Character&&) = delete;

/*---------------------------------------（方法）----------------------------------------------*/
	virtual void Start() = 0;
	virtual void Update() = 0;
	// 回傳角色當前的位置
	[[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }
	// 確認角色是否可見
	[[nodiscard]] bool GetVisibility() const { return m_Visible; }
	// 回傳最大血量
	[[nodiscard]] int GetMaxHp() const { return m_maxHp; }
	// 回傳現在血量
	[[nodiscard]] int GetHp() const { return m_currentHp; }
	std::vector<StatusEffect> GetActiveEffects() { return m_StatusEffects; }

	// 設定圖片與位置
	void SetImage(const std::string& ImagePath);
	// 設定角色的新位置
	void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }

	// 是否發生碰撞
	[[nodiscard]] bool CheckCollides(const std::shared_ptr<Character>& other) const;
	// 承受傷害(扣除生命值)
	void takeDamage(int dmg);
	// 使用當前武器攻擊敵人
	virtual void attack();
	// 移動方法
	virtual void move(glm::vec2 movement) = 0;
	// 判斷是否死亡
	[[nodiscard]] bool isDead() const { return m_currentHp <= 0; }

	void applyStatusEffect(const StatusEffect& effect); // 添加狀態異常
	void updateStatusEffects(double deltaTime);			// 更新狀態異常
	// 武器系統
	[[nodiscard]] Weapon* GetCurrentWeapon() const { return currentWeapon.get(); }
	void RemoveWeapon(Weapon* weapon);
	virtual void AddWeapon(std::unique_ptr<Weapon> weapon);					// 讓子類別控制數量

protected:
	std::string m_ImagePath;
	int m_maxHp;        // 生命上限
	int m_currentHp;    // 當前生命值
	float m_moveSpeed;		// 每秒移動的格數
	int m_aimRange;			// 自動瞄準範圍
	std::unique_ptr<CollisionBox> m_collisionRadius; // 碰撞箱大小
	std::vector<std::unique_ptr<Weapon>> m_Weapons;		// 每個角色都有武器
	std::unique_ptr<Weapon> currentWeapon;				// 目前裝備的武器
	std::vector<StatusEffect> m_StatusEffects;	// 狀態異常
	State m_state = State::STANDING;

private:
	void ResetPosition() { m_Transform.translation = {0, 0}; }
};
#endif //CHARACTER_HPP
