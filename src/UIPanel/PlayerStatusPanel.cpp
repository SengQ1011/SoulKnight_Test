//
// Created by QuzzS on 2025/5/29.
//

#include "UIPanel/PlayerStatusPanel.hpp"

#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Override/nGameObject.hpp"
#include "Scene/SceneManager.hpp"
#include "UIPanel/UISlider.hpp"
#include "Util/Color.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "config.hpp"


void PlayerStatusPanel::Start()
{
	// 面板背景初始化
	m_PanelBackground = std::make_shared<nGameObject>();
	m_PanelBackground->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_playerStatus/background_playerStatusPanel.png"));
	m_PanelBackground->SetZIndex(89.0f);
	m_PanelBackground->m_Transform.translation =
		(glm::vec2(static_cast<float>(PTSD_Config::WINDOW_WIDTH), static_cast<float>(PTSD_Config::WINDOW_HEIGHT)) -
		 m_PanelBackground->GetScaledSize()) *
		glm::vec2(-0.5f, 0.5f);
	m_GameObjects.push_back(m_PanelBackground);

	const glm::vec2 offsetHP = glm::vec2(15.0f, 36.0f);
	const glm::vec2 offsetArmor = glm::vec2(15.0f, 2.0f);
	const glm::vec2 offsetEnergy = glm::vec2(15.0f, -30.0f);

	const glm::vec2 offsetText = glm::vec2(9.0f, 0.0f);

	// HP的監聽function
	const auto listenPlayerHP = [this]() -> float
	{
		const auto healthComp = m_PlayerHealthComponent.lock();
		if (!healthComp)
			return -1.0f;

		const auto maxHP = static_cast<float>(healthComp->GetMaxHp());
		if (maxHP <= 0.0f)
			return -1.0f;

		return static_cast<float>(healthComp->GetCurrentHp()) / maxHP;
	};

	// 護甲的監聽function
	const auto listenPlayerArmor = [this]() -> float
	{
		const auto healthComp = m_PlayerHealthComponent.lock();
		if (!healthComp)
			return -1.0f;

		const auto maxArmor = static_cast<float>(healthComp->GetMaxArmor());
		if (maxArmor <= 0.0f)
			return -1.0f;

		return static_cast<float>(healthComp->GetCurrentArmor()) / maxArmor;
	};

	// 能量的監聽function
	const auto listenPlayerEnergy = [this]() -> float
	{
		const auto healthComp = m_PlayerHealthComponent.lock();
		if (!healthComp)
			return -1.0f;

		const auto maxEnergy = static_cast<float>(healthComp->GetMaxEnergy());
		if (maxEnergy <= 0.0f)
			return -1.0f;

		return static_cast<float>(healthComp->GetCurrentEnergy()) / maxEnergy;
	};

	// 玩家HP的軌道
	const auto trackHP = std::make_shared<nGameObject>();
	trackHP->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_playerStatus/track_hp.png"));
	m_GameObjects.push_back(trackHP);

	// 玩家護甲的軌道
	const auto trackArmor = std::make_shared<nGameObject>();
	trackArmor->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_playerStatus/track_armor.png"));
	m_GameObjects.push_back(trackArmor);

	// 玩家能量的軌道
	const auto trackEnergy = std::make_shared<nGameObject>();
	trackEnergy->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_playerStatus/track_enegy.png"));
	m_GameObjects.push_back(trackEnergy);

	// HP滑桿
	m_SliderPlayerHP = std::make_shared<UISlider>(listenPlayerHP, trackHP, glm::vec2(3.0f, 4.0f), false);
	m_SliderPlayerHP->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_playerStatus/groove_status.png"));
	m_SliderPlayerHP->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_SliderPlayerHP->m_Transform.translation = m_PanelBackground->m_Transform.translation + offsetHP;
	m_SliderPlayerHP->m_Transform.scale = glm::vec2(0.963f, 0.714f);
	m_SliderPlayerHP->Start();
	m_GameObjects.push_back(m_SliderPlayerHP);

	// 護甲滑桿
	m_SliderPlayerArmor = std::make_shared<UISlider>(listenPlayerArmor, trackArmor, glm::vec2(3.0f, 4.0f), false);
	m_SliderPlayerArmor->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_playerStatus/groove_status.png"));
	m_SliderPlayerArmor->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_SliderPlayerArmor->m_Transform.translation = m_PanelBackground->m_Transform.translation + offsetArmor;
	m_SliderPlayerArmor->m_Transform.scale = glm::vec2(0.963f, 0.714f);
	m_SliderPlayerArmor->Start();
	m_GameObjects.push_back(m_SliderPlayerArmor);

	// 能量滑桿
	m_SliderPlayerEnergy = std::make_shared<UISlider>(listenPlayerEnergy, trackEnergy, glm::vec2(3.0f, 4.0f), false);
	m_SliderPlayerEnergy->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_playerStatus/groove_status.png"));
	m_SliderPlayerEnergy->SetZIndex(m_PanelBackground->GetZIndex() + 0.1f);
	m_SliderPlayerEnergy->m_Transform.translation = m_PanelBackground->m_Transform.translation + offsetEnergy;
	m_SliderPlayerEnergy->m_Transform.scale = glm::vec2(0.963f, 0.714f);
	m_SliderPlayerEnergy->Start();
	m_GameObjects.push_back(m_SliderPlayerEnergy);

	// HP數值文字
	m_TextPlayerHP = std::make_shared<nGameObject>();
	m_TextPlayerHP->SetDrawable(ImagePoolManager::GetInstance().GetText(RESOURCE_DIR "/Font/ariblk.ttf", 16, "0/7",
															 Util::Color::FromRGB(255, 255, 255), false));
	m_TextPlayerHP->SetZIndex(m_SliderPlayerHP->GetZIndex() + 0.2f);
	m_TextPlayerHP->m_Transform.translation = m_SliderPlayerHP->m_Transform.translation + offsetText;
	m_TextPlayerHP->m_Transform.scale = glm::vec2(1.0f, 1.0f);
	m_GameObjects.push_back(m_TextPlayerHP);

	// 護甲數值文字
	m_TextPlayerArmor = std::make_shared<nGameObject>();
	m_TextPlayerArmor->SetDrawable(ImagePoolManager::GetInstance().GetText(RESOURCE_DIR "/Font/ariblk.ttf", 16, "0/8",
																Util::Color::FromRGB(255, 255, 255), false));
	m_TextPlayerArmor->SetZIndex(m_SliderPlayerArmor->GetZIndex() + 0.2f);
	m_TextPlayerArmor->m_Transform.translation = m_SliderPlayerArmor->m_Transform.translation + offsetText;
	m_TextPlayerArmor->m_Transform.scale = glm::vec2(1.0f, 1.0f);
	m_GameObjects.push_back(m_TextPlayerArmor);

	// 能量數值文字
	m_TextPlayerEnergy = std::make_shared<nGameObject>();
	m_TextPlayerEnergy->SetDrawable(ImagePoolManager::GetInstance().GetText(RESOURCE_DIR "/Font/ariblk.ttf", 16, "0/180",
																 Util::Color::FromRGB(255, 255, 255), false));
	m_TextPlayerEnergy->SetZIndex(m_SliderPlayerEnergy->GetZIndex() + 0.2f);
	m_TextPlayerEnergy->m_Transform.translation = m_SliderPlayerEnergy->m_Transform.translation + offsetText;
	m_TextPlayerEnergy->m_Transform.scale = glm::vec2(1.0f, 1.0f);
	m_GameObjects.push_back(m_TextPlayerEnergy);

	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		const auto renderer = scene->GetRoot().lock();
		renderer->AddChild(m_PanelBackground);
		renderer->AddChild(m_SliderPlayerHP);
		renderer->AddChild(m_SliderPlayerArmor);
		renderer->AddChild(m_SliderPlayerEnergy);
		renderer->AddChild(trackHP);
		renderer->AddChild(trackArmor);
		renderer->AddChild(trackEnergy);
		renderer->AddChild(m_TextPlayerHP);
		renderer->AddChild(m_TextPlayerArmor);
		renderer->AddChild(m_TextPlayerEnergy);
	}

	Show();
}

void PlayerStatusPanel::Update()
{
	const glm::vec2 offsetHP = glm::vec2(15.0f, 36.0f);
	const glm::vec2 offsetArmor = glm::vec2(15.0f, 2.0f);
	const glm::vec2 offsetEnergy = glm::vec2(15.0f, -30.0f);

	const glm::vec2 offsetText = glm::vec2(9.0f, 0.0f);

	m_SliderPlayerHP->m_Transform.translation = m_PanelBackground->m_Transform.translation + offsetHP;
	m_SliderPlayerArmor->m_Transform.translation = m_PanelBackground->m_Transform.translation + offsetArmor;
	m_SliderPlayerEnergy->m_Transform.translation = m_PanelBackground->m_Transform.translation + offsetEnergy;

	// 更新文字位置
	m_TextPlayerHP->m_Transform.translation = m_SliderPlayerHP->m_Transform.translation + offsetText;
	m_TextPlayerArmor->m_Transform.translation = m_SliderPlayerArmor->m_Transform.translation + offsetText;
	m_TextPlayerEnergy->m_Transform.translation = m_SliderPlayerEnergy->m_Transform.translation + offsetText;

	// 更新文字內容
	const auto healthComp = m_PlayerHealthComponent.lock();
	if (healthComp)
	{
		// 更新HP文字
		const std::string hpText =
			std::to_string(healthComp->GetCurrentHp()) + "/" + std::to_string(healthComp->GetMaxHp());
		auto hpTextDrawable = std::dynamic_pointer_cast<Util::Text>(m_TextPlayerHP->GetDrawable());
		if (hpTextDrawable)
		{
			hpTextDrawable->SetText(hpText);
		}

		// 更新護甲文字
		const std::string armorText =
			std::to_string(healthComp->GetCurrentArmor()) + "/" + std::to_string(healthComp->GetMaxArmor());
		auto armorTextDrawable = std::dynamic_pointer_cast<Util::Text>(m_TextPlayerArmor->GetDrawable());
		if (armorTextDrawable)
		{
			armorTextDrawable->SetText(armorText);
		}

		// 更新能量文字
		const std::string energyText =
			std::to_string(healthComp->GetCurrentEnergy()) + "/" + std::to_string(healthComp->GetMaxEnergy());
		auto energyTextDrawable = std::dynamic_pointer_cast<Util::Text>(m_TextPlayerEnergy->GetDrawable());
		if (energyTextDrawable)
		{
			energyTextDrawable->SetText(energyText);
		}
	}

	DrawDebugUI();
	for (const std::shared_ptr<nGameObject> &gameObject : m_GameObjects)
	{
		gameObject->Update();
	}
}

void PlayerStatusPanel::DrawDebugUI()
{
	ImGui::Begin("Player Status Panel Debug");

	// === Background Settings ===
	if (ImGui::CollapsingHeader("Background", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 size = m_PanelBackground->m_Transform.scale * m_PanelBackground->GetImageSize();
		static glm::vec2 pos = m_PanelBackground->m_Transform.translation;
		static bool sizeChanged = false, posChanged = false;

		ImGui::InputFloat("Width##bg", &size.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Height##bg", &size.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Pos X##bg", &pos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Pos Y##bg", &pos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;

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
	}

	// === HP Slider Settings ===
	if (ImGui::CollapsingHeader("HP Slider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 size = m_SliderPlayerHP->m_Transform.scale * m_SliderPlayerHP->GetImageSize();
		static glm::vec2 pos = m_SliderPlayerHP->m_Transform.translation;
		static glm::vec2 pivot = m_SliderPlayerHP->GetPivot();
		static bool sizeChanged = false, posChanged = false, pivotChanged = false;

		ImGui::InputFloat("Width##hp", &size.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Height##hp", &size.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Pos X##hp", &pos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Pos Y##hp", &pos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Pivot X##hp", &pivot.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			pivotChanged = true;
		ImGui::InputFloat("Pivot Y##hp", &pivot.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			pivotChanged = true;

		if (sizeChanged)
		{
			m_SliderPlayerHP->m_Transform.scale = size / m_SliderPlayerHP->GetImageSize();
			sizeChanged = false;
		}
		if (posChanged)
		{
			m_SliderPlayerHP->m_Transform.translation = pos;
			posChanged = false;
		}
		if (pivotChanged)
		{
			m_SliderPlayerHP->SetPivot(pivot);
			pivotChanged = false;
		}

		ImGui::LabelText("scale.x", "%.3f", m_SliderPlayerHP->m_Transform.scale.x);
		ImGui::LabelText("scale.y", "%.3f", m_SliderPlayerHP->m_Transform.scale.y);
	}

	// === Armor Slider Settings ===
	if (ImGui::CollapsingHeader("Armor Slider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 size = m_SliderPlayerArmor->m_Transform.scale * m_SliderPlayerArmor->GetImageSize();
		static glm::vec2 pos = m_SliderPlayerArmor->m_Transform.translation;
		static glm::vec2 pivot = m_SliderPlayerArmor->GetPivot();
		static bool sizeChanged = false, posChanged = false, pivotChanged = false;

		ImGui::InputFloat("Width##armor", &size.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Height##armor", &size.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Pos X##armor", &pos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Pos Y##armor", &pos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Pivot X##armor", &pivot.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			pivotChanged = true;
		ImGui::InputFloat("Pivot Y##armor", &pivot.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			pivotChanged = true;

		if (sizeChanged)
		{
			m_SliderPlayerArmor->m_Transform.scale = size / m_SliderPlayerArmor->GetImageSize();
			sizeChanged = false;
		}
		if (posChanged)
		{
			m_SliderPlayerArmor->m_Transform.translation = pos;
			posChanged = false;
		}
		if (pivotChanged)
		{
			m_SliderPlayerArmor->SetPivot(pivot);
			pivotChanged = false;
		}

		ImGui::LabelText("scale.x", "%.3f", m_SliderPlayerArmor->m_Transform.scale.x);
		ImGui::LabelText("scale.y", "%.3f", m_SliderPlayerArmor->m_Transform.scale.y);
	}

	// === Energy Slider Settings ===
	if (ImGui::CollapsingHeader("Energy Slider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 size = m_SliderPlayerEnergy->m_Transform.scale * m_SliderPlayerEnergy->GetImageSize();
		static glm::vec2 pos = m_SliderPlayerEnergy->m_Transform.translation;
		static glm::vec2 pivot = m_SliderPlayerEnergy->GetPivot();
		static bool sizeChanged = false, posChanged = false, pivotChanged = false;

		ImGui::InputFloat("Width##energy", &size.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Height##energy", &size.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			sizeChanged = true;
		ImGui::InputFloat("Pos X##energy", &pos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Pos Y##energy", &pos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Pivot X##energy", &pivot.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			pivotChanged = true;
		ImGui::InputFloat("Pivot Y##energy", &pivot.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			pivotChanged = true;

		if (sizeChanged)
		{
			m_SliderPlayerEnergy->m_Transform.scale = size / m_SliderPlayerEnergy->GetImageSize();
			sizeChanged = false;
		}
		if (posChanged)
		{
			m_SliderPlayerEnergy->m_Transform.translation = pos;
			posChanged = false;
		}
		if (pivotChanged)
		{
			m_SliderPlayerEnergy->SetPivot(pivot);
			pivotChanged = false;
		}

		ImGui::LabelText("scale.x", "%.3f", m_SliderPlayerEnergy->m_Transform.scale.x);
		ImGui::LabelText("scale.y", "%.3f", m_SliderPlayerEnergy->m_Transform.scale.y);
	}

	// === HP Text Settings ===
	if (ImGui::CollapsingHeader("HP Text", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 pos = m_TextPlayerHP->m_Transform.translation;
		static glm::vec2 scale = m_TextPlayerHP->m_Transform.scale;
		static bool posChanged = false, scaleChanged = false;

		ImGui::InputFloat("Pos X##hptext", &pos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Pos Y##hptext", &pos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Scale X##hptext", &scale.x, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			scaleChanged = true;
		ImGui::InputFloat("Scale Y##hptext", &scale.y, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			scaleChanged = true;

		if (posChanged)
		{
			m_TextPlayerHP->m_Transform.translation = pos;
			posChanged = false;
		}
		if (scaleChanged)
		{
			m_TextPlayerHP->m_Transform.scale = scale;
			scaleChanged = false;
		}
	}

	// === Armor Text Settings ===
	if (ImGui::CollapsingHeader("Armor Text", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 pos = m_TextPlayerArmor->m_Transform.translation;
		static glm::vec2 scale = m_TextPlayerArmor->m_Transform.scale;
		static bool posChanged = false, scaleChanged = false;

		ImGui::InputFloat("Pos X##armortext", &pos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Pos Y##armortext", &pos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Scale X##armortext", &scale.x, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			scaleChanged = true;
		ImGui::InputFloat("Scale Y##armortext", &scale.y, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			scaleChanged = true;

		if (posChanged)
		{
			m_TextPlayerArmor->m_Transform.translation = pos;
			posChanged = false;
		}
		if (scaleChanged)
		{
			m_TextPlayerArmor->m_Transform.scale = scale;
			scaleChanged = false;
		}
	}

	// === Energy Text Settings ===
	if (ImGui::CollapsingHeader("Energy Text", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static glm::vec2 pos = m_TextPlayerEnergy->m_Transform.translation;
		static glm::vec2 scale = m_TextPlayerEnergy->m_Transform.scale;
		static bool posChanged = false, scaleChanged = false;

		ImGui::InputFloat("Pos X##energytext", &pos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Pos Y##energytext", &pos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			posChanged = true;
		ImGui::InputFloat("Scale X##energytext", &scale.x, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			scaleChanged = true;
		ImGui::InputFloat("Scale Y##energytext", &scale.y, 0.1f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			scaleChanged = true;

		if (posChanged)
		{
			m_TextPlayerEnergy->m_Transform.translation = pos;
			posChanged = false;
		}
		if (scaleChanged)
		{
			m_TextPlayerEnergy->m_Transform.scale = scale;
			scaleChanged = false;
		}
	}

	ImGui::End();
}
