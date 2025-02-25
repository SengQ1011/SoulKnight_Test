//
// Created by QuzzS on 2025/2/25.
//

#ifndef TEXTOBJECT_HPP
#define TEXTOBJECT_HPP

#include "Tool.hpp"
#include "Util/GameObject.hpp"
#include "Util/Logger.hpp"
#include "Util/Text.hpp"

class TextObjectUI : public Util::GameObject
{
public:
	TextObjectUI(const std::string& font , const std::string& title);

	~TextObjectUI() override = default;

	void Init( int size, float index = 0, glm::vec2 pivot = {0,0} );

	[[nodiscard]] Tool::Rectangle GetBound() const { return m_Bound; } //TODO:爲什麽回傳自定struct需要[[nodiscard]]？

	void SetText( const std::string& text) const;

	void SetColor(const Util::Color &color) const;

protected:
	int m_Size = 100; //提醒調大小
	std::string m_Font;
	std::string m_Title;
	Tool::Rectangle m_Bound{};
	std::shared_ptr<Util::Text> p_Text;
};

#endif //TEXTOBJECT_HPP
