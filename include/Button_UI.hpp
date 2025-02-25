//
// Created by QuzzS on 2025/2/25.
//

#ifndef BUTTON_UI_HPP
#define BUTTON_UI_HPP

#include "Util/GameObject.hpp"
#include "Util/SFX.hpp"
#include "Util/Text.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Image.hpp"

#include "TextObject_UI.hpp"
#include "Tool.hpp"
#include "Util/Logger.hpp"

//
// class ButtonUI : public Util::GameObject
// {
// public:
// 	ButtonUI() = default;
//
// 	ButtonUI(float zIndex, glm::vec2 pos, const std::string& font, const std::string& title, int size, std::shared_ptr<Util::Image> image);
//
// 	~ButtonUI() override = default;
//
// 	bool IfClickBtn();
//
// private:
// 	Util::SFX m_ClickSound = Util::SFX(RESOURCE_DIR"/UI/fx_btn1.wav");
// 	std::shared_ptr<TextObjectUI> m_Text = nullptr;
// 	std::shared_ptr<TextObjectUI> m_TextShadow = nullptr;
// 	std::shared_ptr<Util::Image> m_Image = nullptr;
//
// 	Tool::Rectangle m_Bound{};
// };

#endif //BUTTON_UI_HPP
