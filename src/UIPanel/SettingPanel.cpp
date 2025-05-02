//
// Created by QuzzS on 2025/5/1.
//

#include "UIPanel/SettingPanel.hpp"

#include "Override/nGameObject.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

void SettingPanel::Start()
{
	std::shared_ptr<nGameObject> gameObject = std::make_shared<nGameObject>();
	LOG_DEBUG("SettingPanel::Start1");
	gameObject->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR "/UI/MenuPanel.png"));
	gameObject->SetZIndexType(CUSTOM);
	gameObject->SetZIndex(80.0f);
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		const auto a = scene->GetRoot().lock();
		a->AddChild(gameObject);
	}
	m_GameObjects.push_back(gameObject);
}

void SettingPanel::Update()
{
	for (const std::shared_ptr<nGameObject>& gameObject : m_GameObjects)
	{
		gameObject->Update();
	}
}

void SettingPanel::DrawDebugUI()
{

	// // 可放在主 ImGui Debug 面板中
	// // size 控制視窗大小
	// static ImVec2 size = ImVec2(640.0f, 400.0f);
	// static ImVec2 pos = ImVec2(0,0);
	// // pos 控制視窗位置
	// static bool posChangedByInput = false;
	// static bool sizeChangedByInput = false;
	//
	// // 音量控制
	// static float masterVolume = 0;
	// static float SFX_Volume = 0;
	// static float BGM_Volume = 0;
	//
	// if (sizeChangedByInput)
	// {
	// 	ImGui::SetNextWindowSize(size, ImGuiCond_Always);
	// 	sizeChangedByInput = false;
	// }
	// if (posChangedByInput)
	// {
	// 	ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
	// 	posChangedByInput = false;
	// }
	//
	// ImGui::Begin("Settings");
	//
	// ImGui::InputFloat("Setting Width", &size.x, 1.0f);
	// if (ImGui::IsItemDeactivatedAfterEdit()) sizeChangedByInput = true;
	// ImGui::InputFloat("Setting Height", &size.y, 1.0f);
	// if (ImGui::IsItemDeactivatedAfterEdit()) sizeChangedByInput = true;
	//
	// ImGui::InputFloat("Setting x", &pos.x, 1.0f);
	// if (ImGui::IsItemDeactivatedAfterEdit()) posChangedByInput = true;
	// ImGui::InputFloat("Setting y", &pos.y, 1.0f);
	// if (ImGui::IsItemDeactivatedAfterEdit()) posChangedByInput = true;
	//
	// if (!sizeChangedByInput) size = ImGui::GetWindowSize();
	// if (!posChangedByInput) pos = ImGui::GetWindowPos();
	//
	// // ImGui::Text("Current Position: x = %.1f, y = %.1f", pos.x, pos.y);
	// // ImGui::Text("Current Size: %.1f x %.1f", size.x, size.y);
	//
	// if (ImGui::CollapsingHeader("Volume Setting",ImGuiTreeNodeFlags_DefaultOpen))
	// {
	// 	ImGui::SliderFloat("MasterVolume", &masterVolume, 0.0f, 100.0f, "%.0f%%");
	// 	ImGui::SliderFloat("SFXVolume", &SFX_Volume, 0.0f, 100.0f, "%.0f%%");
	// 	ImGui::SliderFloat("BGMVolume", &BGM_Volume, 0.0f, 100.0f, "%.0f%%");
	// 	SetMasterVolume(masterVolume*0.01f);
	// 	SetSFXVolume(SFX_Volume*0.01f);
	// 	SetBGMVolume(BGM_Volume*0.01f);
	// }
	//
	// if (ImGui::CollapsingHeader("VolumeOutput",ImGuiTreeNodeFlags_DefaultOpen))
	// {
	// 	ImGui::Text("Master Volume: %.0f%%", GetMasterVolume() * 100.0f);
	// 	ImGui::Text("SFX Volume: %.0f%%", GetSFXVolume() * 100.0f);
	// 	ImGui::Text("BGM Volume: %.0f%%", GetBGMVolume() * 100.0f);
	// }
	// ImGui::End();

}

