//
// Created by QuzzS on 2025/2/25.
//

#include "TextObject_UI.hpp"

TextObjectUI::TextObjectUI( const std::string &font, const std::string &title )
{
	m_Font = font;
	m_Title = title;
}

void TextObjectUI::Init(const int size, const float index, const glm::vec2 pivot )
{
	m_Size = size;
	m_ZIndex = index;
	m_Pivot = pivot;
	p_Text = std::make_shared<Util::Text>(m_Font, m_Size, m_Title);
	m_Bound = Tool::Bound(m_Pivot, p_Text->GetSize());
	p_Text->SetColor(Util::Color::FromRGB(255,255,255));
	SetDrawable(p_Text);
}


void TextObjectUI::SetText(const std::string &text) const {
	p_Text->SetText(text);
}

void TextObjectUI::SetColor( const Util::Color &color ) const {
	p_Text->SetColor(color);
}
