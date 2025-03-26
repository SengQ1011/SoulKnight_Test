//
// Created by QuzzS on 2025/3/4.
//

#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <vector>
#include "Animation.hpp"

#include "Override/nGameObject.hpp"
#include "Weapon/Weapon.hpp"
#include "Tool/CollisionBox.hpp"
#include "Components/Component.hpp"
#include <memory>
#include <unordered_map>
#include "Util/Image.hpp"

class Character : public nGameObject {
public:
	// 建構子+指定角色的圖片(explicit：防止單參數構造函數進行隱式轉換)
	explicit Character(int maxHp,float moveSpeed, std::unique_ptr<CollisionBox> collisionBox);
	~Character() override = default;

	// delete function--> 禁止 Character 被複製或移動,確保遊戲角色的唯一性
	Character(const Character&) = delete;
	Character(Character&&) = delete;
	Character& operator=(const Character&) = delete;
	Character& operator=(Character&&) = delete;

/*---------------------------------------（方法）----------------------------------------------*/
	virtual void Start() = 0;

	/* ---Getter--- */
	[[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }		// 回傳角色當前的位置
	[[nodiscard]] bool GetVisibility() const { return m_Visible; }								// 確認角色是否可見
	[[nodiscard]] int GetMaxHp() const { return m_maxHp; }										// 回傳最大血量
	[[nodiscard]] int GetHp() const { return m_currentHp; }										// 回傳現在血量
	[[nodiscard]] bool isDead() const { return m_currentHp <= 0; }									// 判斷是否死亡

	[[nodiscard]] bool CheckCollides(const std::shared_ptr<Character>& other) const;			// 是否發生碰撞
	// CollisionBox 的實際位置
	CollisionBox GetCollisionBox() const {
		return CollisionBox(m_collisionBox->GetWidth(),
							m_collisionBox->GetHeight(),
							m_collisionBox->GetX(this->m_WorldCoord.x),
							m_collisionBox->GetY(this->m_WorldCoord.y));
	}



	/* ---Setter--- */
	void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }			// 設定角色的新位置
	void takeDamage(int dmg);	// 承受傷害(扣除生命值

	void UpdateComponents(float deltaTime);


protected:
	int m_maxHp;        // 生命上限
	int m_currentHp;    // 當前生命值
	float m_moveSpeed;		// 每秒移動的格數

	std::unique_ptr<CollisionBox> m_collisionBox; // 碰撞箱大小

private:
	void ResetPosition() { m_Transform.translation = {0, 0}; }
};
#endif //CHARACTER_HPP
