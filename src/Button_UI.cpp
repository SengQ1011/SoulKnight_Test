//
// Created by QuzzS on 2025/2/25.
//

// #include "Button_UI.hpp"
//
// ButtonUI::ButtonUI(float zIndex, glm::vec2 pos, const std::string& font, const std::string& title, int size, std::shared_ptr<Util::Image> image)
// {
// 	m_Text = std::make_shared<TextObjectUI>(font, title, size);
// 	m_TextShadow = std::make_shared<TextObjectUI>(font, title, size);
//
// 	m_ZIndex = zIndex;
// 	m_Text->SetZIndex(m_ZIndex + 2);
// 	m_TextShadow->SetZIndex(m_ZIndex + 1);
//
// 	SetPivot(pos);
// 	m_Text->SetPivot(pos+glm::vec2(-11,-3));
// 	m_TextShadow->SetPivot(pos+glm::vec2(-11,-1)); //下移
// 	m_TextShadow->SetColor(Util::Color::FromRGB(100,100,100,230)); //灰階
//
// 	m_Image = std::move(image);
// 	m_Bound = Tool::Bound(GetPivot(),m_Image->GetSize()*glm::vec2(1.7,1.7));
//
// 	//m_Transform.scale = glm::vec2(1.7,1.7);
// 	AddChild(m_Text);
// 	AddChild(m_TextShadow);
// 	SetDrawable(m_Image);
//
//
// 	LOG_DEBUG("i{},{}; {},{}",m_Bound.v_Max.x,m_Bound.v_Max.y,m_Bound.v_Min.x,m_Bound.v_Min.y);
//
// }
//
//
// bool ButtonUI::IfClickBtn()
// {
// 	glm::vec2 mouse = Tool::GetMouseCoord();
//
// 	//光標在按鈕範圍外
// 	if((mouse.x > m_Bound.v_Max.x) || (mouse.y > m_Bound.v_Max.y) || (mouse.x < m_Bound.v_Min.x) || (mouse.y < m_Bound.v_Min.y))
// 	{
// 		return false;
// 	}
//
//
// 	if(Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB))
// 	{
// 		m_ClickSound.Play();
// 		return true;
// 	}
// 	return false;
// }



