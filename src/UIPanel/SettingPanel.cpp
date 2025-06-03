//
// Created by QuzzS on 2025/5/1.
//

#include "UIPanel/SettingPanel.hpp"

#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Override/nGameObject.hpp"
#include "Scene/SceneManager.hpp"
#include "UIPanel/UIButton.hpp"
#include "UIPanel/UISlider.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

void SettingPanel::Start()
{
	// 位置偏移量定義
	const glm::vec2 backgroundOffset = glm::vec2(0.0f, 22.0f);
	const glm::vec2 sliderOffset1 = glm::vec2(50.0f, 92.0f);
	const glm::vec2 sliderOffset2 = glm::vec2(50.0f, -33.0f);
	const glm::vec2 sliderOffset3 = glm::vec2(50.0f, -158.0f);

	m_PanelBackground = std::make_shared<nGameObject>();
	m_PanelBackground->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/background_menuPanel.png"));
	m_PanelBackground->SetZIndex(80.0f);
	m_PanelBackground->m_Transform.translation = backgroundOffset;
	m_GameObjects.push_back(m_PanelBackground);

	// 主音量的監聽和控制function
	const auto controlMasterVoice = [](const float newValue) { AudioManager::GetInstance().SetMasterVolume(newValue); };
	const auto listenMasterVoice = []() -> float { return AudioManager::GetInstance().GetMasterVolume(); };

	// 主音量的軌道
	const auto track1 = std::make_shared<nGameObject>();
	track1->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/ui_progress_blue.png"));
	m_GameObjects.push_back(track1);

	// 主音量滑桿
	m_SliderMasterVolume = std::make_shared<UISlider>(listenMasterVoice, track1, glm::vec2(7.0f,4.0f), true,
													  RESOURCE_DIR "/UI/ui_settingPanel/button_volume.png", controlMasterVoice);
	m_SliderMasterVolume->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/ui_progress_bg.png"));
	m_SliderMasterVolume->SetZIndex(m_PanelBackground->GetZIndex() + 1.0f);
	m_SliderMasterVolume->m_Transform.translation = sliderOffset1;
	m_SliderMasterVolume->Start();
	const auto slider_button1 = m_SliderMasterVolume->GetButton();
	m_GameObjects.push_back(slider_button1);
	m_GameObjects.push_back(m_SliderMasterVolume);

	const auto controlBGMVoice = [](const float newValue) { AudioManager::GetInstance().SetBGMVolume(newValue); };
	const auto listenBGMVoice = []() -> float { return AudioManager::GetInstance().GetBGMVolume(); };

	const auto track2 = std::make_shared<nGameObject>();
	track2->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/ui_progress_blue.png"));
	m_GameObjects.push_back(track2);

	m_SliderBGMVolume = std::make_shared<UISlider>(listenBGMVoice, track2, glm::vec2(7.0f,4.0f), true,
												   RESOURCE_DIR "/UI/ui_settingPanel/button_volume.png", controlBGMVoice);
	m_SliderBGMVolume->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/ui_progress_bg.png"));
	m_SliderBGMVolume->SetZIndex(m_PanelBackground->GetZIndex() + 1.0f);
	m_SliderBGMVolume->m_Transform.translation = sliderOffset2;
	m_SliderBGMVolume->Start();
	const auto slider_button2 = m_SliderBGMVolume->GetButton();
	m_GameObjects.push_back(slider_button2);
	m_GameObjects.push_back(m_SliderBGMVolume);

	const auto controlSFXVoice = [](const float newValue) { AudioManager::GetInstance().SetSFXVolume(newValue); };
	const auto listenSFXVoice = []() -> float { return AudioManager::GetInstance().GetSFXVolume(); };

	const auto track3 = std::make_shared<nGameObject>();
	track3->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/ui_progress_blue.png"));
	m_GameObjects.push_back(track3);

	m_SliderSFXVolume = std::make_shared<UISlider>(listenSFXVoice, track3, glm::vec2(7.0f,4.0f), true,
												   RESOURCE_DIR "/UI/ui_settingPanel/button_volume.png", controlSFXVoice);
	m_SliderSFXVolume->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/ui_progress_bg.png"));
	m_SliderSFXVolume->SetZIndex(m_PanelBackground->GetZIndex() + 1.0f);
	m_SliderSFXVolume->m_Transform.translation = sliderOffset3;
	m_SliderSFXVolume->Start();
	const auto slider_button3 = m_SliderSFXVolume->GetButton();
	m_GameObjects.push_back(slider_button3);
	m_GameObjects.push_back(m_SliderSFXVolume);

	std::function<void()> button_function = [this]() { this->Hide(); };
	m_CloseButton = std::make_shared<UIButton>(button_function, false);
	m_CloseButton->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_settingPanel/button_close.png"));
	m_CloseButton->SetZIndex(m_PanelBackground->GetZIndex() + 1.0f);
	m_CloseButton->m_Transform.translation = glm::vec2(335.0f, 219.0f);
	m_GameObjects.push_back(m_CloseButton);

	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		const auto renderer = scene->GetRoot().lock();
		renderer->AddChild(m_PanelBackground);
		renderer->AddChild(m_CloseButton);
		renderer->AddChild(track1);
		renderer->AddChild(track2);
		renderer->AddChild(track3);
		renderer->AddChild(slider_button1);
		renderer->AddChild(slider_button2);
		renderer->AddChild(slider_button3);
		renderer->AddChild(m_SliderMasterVolume);
		renderer->AddChild(m_SliderBGMVolume);
		renderer->AddChild(m_SliderSFXVolume);
	}

	Hide();
}

void SettingPanel::Update()
{
	DrawDebugUI();
	for (const std::shared_ptr<nGameObject> &gameObject : m_GameObjects)
	{
		gameObject->Update();
	}
}

void SettingPanel::DrawDebugUI()
{

	ImGui::Begin("UI Settings");

	// === Background Settings ===
	if (ImGui::CollapsingHeader("Background Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 size = glm::vec2(760.0f, 528.0f);
		static glm::vec2 pos = glm::vec2(0, 0);
		static bool posChanged = false, sizeChanged = false;

		if (sizeChanged)
		{
			m_PanelBackground->m_Transform.scale = size / m_PanelBackground->GetImageSize();
			sizeChanged = false;
		}
		if (posChanged)
		{
			m_PanelBackground->m_Transform.translation = pos;
			posChanged = false;
		}

		ImGui::InputFloat("Setting Width##bg", &size.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Setting Height##bg", &size.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Setting x##bg", &pos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Setting y##bg", &pos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;

		if (!sizeChanged)
			size = m_PanelBackground->m_Transform.scale * m_PanelBackground->GetImageSize();
		if (!posChanged)
			pos = m_PanelBackground->m_Transform.translation;

		ImGui::LabelText("scale.x", "%.3f", m_PanelBackground->m_Transform.scale.x);
		ImGui::LabelText("scale.y", "%.3f", m_PanelBackground->m_Transform.scale.y);
	}

	// === Slider Settings ===
	if (ImGui::CollapsingHeader("Slider Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 size = glm::vec2(760.0f, 528.0f);
		static glm::vec2 pos = glm::vec2(0, 0);
		static glm::vec2 pivot = glm::vec2(0, 0);
		static bool posChanged = false, sizeChanged = false, pivotChanged = false;

		if (sizeChanged)
		{
			m_SliderMasterVolume->m_Transform.scale = size / m_SliderMasterVolume->GetImageSize();
			sizeChanged = false;
		}
		if (posChanged)
		{
			m_SliderMasterVolume->m_Transform.translation = pos;
			posChanged = false;
		}
		if (pivotChanged)
		{
			m_SliderMasterVolume->SetPivot(pivot);
			pivotChanged = false;
		}

		ImGui::InputFloat("Setting Width##slider", &size.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Setting Height##slider", &size.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Setting x##slider", &pos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Setting y##slider", &pos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Setting pivot x", &pivot.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			pivotChanged = true;
		ImGui::InputFloat("Setting pivot y", &pivot.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			pivotChanged = true;

		if (!sizeChanged)
			size = m_SliderMasterVolume->m_Transform.scale * m_SliderMasterVolume->GetImageSize();
		if (!posChanged)
			pos = m_SliderMasterVolume->m_Transform.translation;
		if (!pivotChanged)
			pivot = m_SliderMasterVolume->GetPivot();

		ImGui::LabelText("scale.x", "%.3f", m_SliderMasterVolume->m_Transform.scale.x);
		ImGui::LabelText("scale.y", "%.3f", m_SliderMasterVolume->m_Transform.scale.y);
		ImGui::LabelText("pivot.x", "%.3f", m_SliderMasterVolume->GetPivot().x);
		ImGui::LabelText("pivot.y", "%.3f", m_SliderMasterVolume->GetPivot().y);
		ImGui::LabelText("imagesize.x", "%.3f", m_SliderMasterVolume->GetImageSize().x);
		ImGui::LabelText("imagesize.y", "%.3f", m_SliderMasterVolume->GetImageSize().y);
		ImGui::LabelText("imageScaledSize.x", "%.3f", m_SliderMasterVolume->GetScaledSize().x);
		ImGui::LabelText("imageScaledSize.y", "%.3f", m_SliderMasterVolume->GetScaledSize().y);
	}

	ImGui::End();
}
