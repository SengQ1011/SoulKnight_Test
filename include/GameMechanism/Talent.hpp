//
// Created by QuzzS on 2025/3/4.
//

#ifndef TALENT_HPP
#define TALENT_HPP
// #include <string>
// #include <functional>
// #include "Creature/Character.hpp"
#include "pch.hpp"
#include <functional>

class Character;

class Talent {
public:
	Talent(const int id, const std::string& name, const std::string& iconPath,
		   std::function<void(Character&)> applyFunc,
		   std::function<void(Character&)> undoFunc)
		: m_id(id), m_talentName(name), m_iconPath(iconPath),
		  m_applyFunc(applyFunc), m_undoFunc(undoFunc) {}

	//----Getter----
	[[nodiscard]] int GetId() const { return m_id; }
	[[nodiscard]] std::string GetName() const { return m_talentName; }
	[[nodiscard]] std::string GetIconPath() const { return m_iconPath; }

	void Apply(Character& owner) const {
		if (m_applyFunc) m_applyFunc(owner);
	}
	void Undo(Character& owner) const {
		if (m_undoFunc) m_undoFunc(owner);  // 撤銷天賦
	}

private:
	int m_id;
	std::string m_talentName;
	std::string m_iconPath;
	std::function<void(Character&)> m_applyFunc;
	std::function<void(Character&)> m_undoFunc;
};

#endif //TALENT_HPP
