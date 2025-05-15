//
// Created by QuzzS on 2025/3/4.
//

#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "Override/nGameObject.hpp"

class Character final : public nGameObject {
public:
	// 建構子(explicit：防止單參數構造函數進行隱式轉換)
	explicit Character(const std::string &m_name,CharacterType type);
	~Character() override = default;

	// delete function--> 禁止 Character 被複製或移動,確保遊戲角色的唯一性
	Character(const Character&) = delete;
	Character(Character&&) = delete;
	Character& operator=(const Character&) = delete;
	Character& operator=(Character&&) = delete;

	/* ---Getter--- */
	[[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }		// 回傳角色當前的位置
	[[nodiscard]] CharacterType GetType() const { return m_type; }
	[[nodiscard]] std::string GetCharacterName() const { return m_name; }
	std::string GetName() const override { return m_name; }

	/* ---Setter--- */
	void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }			// 設定角色的新位置

private:
	std::string m_name;
	CharacterType m_type;
	void ResetPosition() { m_Transform.translation = {0, 0}; }
};
#endif //CHARACTER_HPP
