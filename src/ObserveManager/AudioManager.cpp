//
// Created by QuzzS on 2025/5/1.
//

#include "ObserveManager/AudioManager.hpp"
#include <algorithm>
#include <fstream>
#include "Util/BGM.hpp"
#include "Util/Logger.hpp"
#include "Util/SFX.hpp"
#include "json.hpp"

#include "Tool/Tool.hpp"

using json = nlohmann::json;

AudioManager& AudioManager::GetInstance() {
    static AudioManager instance;
    return instance;
}

void AudioManager::PlaySFX(const std::string& name) {
    if (m_Muted || m_SFXs.find(name) == m_SFXs.end()) {
        LOG_DEBUG("AudioManager::PlaySFX: No such SFX: " + name);
        return;
    }
    auto& sfx = m_SFXs[name];
    int volume = static_cast<int>(128 * m_MasterVolume * m_SFXVolume);
    sfx->SetVolume(std::clamp(volume, 0, 128));
    sfx->Play();
}

void AudioManager::FadeInSFX(const std::string& name, unsigned int tick, int loop, unsigned int duration) {
    if (m_Muted || m_SFXs.find(name) == m_SFXs.end()) {
        LOG_DEBUG("AudioManager::FadeInSFX: No such SFX: " + name);
        return;
    }
    auto& sfx = m_SFXs[name];
    int volume = static_cast<int>(128 * m_MasterVolume * m_SFXVolume);
    sfx->SetVolume(std::clamp(volume, 0, 128));
    sfx->FadeIn(tick, loop, duration);
}

void AudioManager::PlayBGM(bool loop) {
    if (m_Muted || !m_BGM || m_BGM->GetVolume() == -1) {
        LOG_DEBUG("AudioManager::PlayBGM: No BGM loaded or muted.");
        return;
    }
    int volume = static_cast<int>(128 * m_MasterVolume * m_BGMVolume);
    m_BGM->SetVolume(std::clamp(volume, 0, 128));
    m_BGM->Play(loop ? -1 : 0);
}

void AudioManager::FadeInBGM(int tick, int loop) {
    if (m_Muted || !m_BGM) return;
    int volume = static_cast<int>(128 * m_MasterVolume * m_BGMVolume);
    m_BGM->SetVolume(std::clamp(volume, 0, 128));
    m_BGM->FadeIn(tick, loop);
}

void AudioManager::FadeOutBGM(int tick) {
    if (m_Muted || !m_BGM) return;
    m_BGM->FadeOut(tick);
}

void AudioManager::PauseBGM() {
    if (m_BGM) m_BGM->Pause();
}

void AudioManager::ResumeBGM() {
    if (m_BGM) m_BGM->Resume();
}

void AudioManager::SetMasterVolume(float volume) {
    m_MasterVolume = std::clamp(volume, 0.0f, 1.0f);
	RefreshAllVolumes();
}

void AudioManager::SetSFXVolume(float volume) {
    m_SFXVolume = std::clamp(volume, 0.0f, 1.0f);
	RefreshAllVolumes();
}

void AudioManager::SetBGMVolume(float volume) {
    m_BGMVolume = std::clamp(volume, 0.0f, 1.0f);
	RefreshAllVolumes();
}

void AudioManager::RefreshAllVolumes()
{
	const int sfx_volume = static_cast<int>(128 * m_MasterVolume * m_SFXVolume);
	for (auto& [_, sfx] : m_SFXs)
		sfx->SetVolume(std::clamp(sfx_volume, 0, 128));

	if (m_BGM) {
		const int bgm_volume = static_cast<int>(128 * m_MasterVolume * m_BGMVolume);
		m_BGM->SetVolume(std::clamp(bgm_volume, 0, 128));
	}
}


void AudioManager::Mute(bool mute) {
    m_Muted = mute;
    if (mute) {
        if (m_BGM) m_BGM->SetVolume(0);
        for (auto& [_, sfx] : m_SFXs) sfx->SetVolume(0);
    } else {
        if (m_BGM) {
            int volume = static_cast<int>(128 * m_MasterVolume * m_BGMVolume);
            m_BGM->SetVolume(std::clamp(volume, 0, 128));
        }
        for (auto& [_, sfx] : m_SFXs) {
            int volume = static_cast<int>(128 * m_MasterVolume * m_SFXVolume);
            sfx->SetVolume(std::clamp(volume, 0, 128));
        }
    }
}

void AudioManager::RegisterSFX(const std::string& name, const std::string& path) {
    m_SFXs[name] = std::make_shared<Util::SFX>(path);
}

void AudioManager::RegisterBGM(const std::string& path) {
    m_BGM = std::make_shared<Util::BGM>(path);
}

void AudioManager::LoadFromJson(const std::string& path) {
    std::ifstream inFile(JSON_DIR+path);
    if (!inFile.is_open()) {
        LOG_DEBUG("AudioManager::LoadFromJson: Failed to open file: " + path);
        return;
    }

    json j;
    inFile >> j;

    if (j.contains("sfx")) {
        for (auto& [name, sfxPath] : j["sfx"].items()) {
            RegisterSFX(name, RESOURCE_DIR+sfxPath.get<std::string>());
        }
    }
    if (j.contains("bgm")) {
        for (auto& [name, bgmPath] : j["bgm"].items()) {
            RegisterBGM(RESOURCE_DIR+bgmPath.get<std::string>());
        }
    }
}

void AudioManager::Reset() {
    m_SFXs.clear();
    m_BGM.reset();
}

void AudioManager::DrawDebugUI()
{

	// 可放在主 ImGui Debug 面板中
	// size 控制視窗大小
	static ImVec2 size = ImVec2(640.0f, 400.0f);
	static ImVec2 pos = ImVec2(0,0);
	// pos 控制視窗位置
	static bool posChangedByInput = false;
	static bool sizeChangedByInput = false;

	// 音量控制
	static float masterVolume = 0;
	static float SFX_Volume = 0;
	static float BGM_Volume = 0;

	// 當
	if (sizeChangedByInput)
	{
		ImGui::SetNextWindowSize(size, ImGuiCond_Always);
		sizeChangedByInput = false;
	}
	if (posChangedByInput)
	{
		ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
		posChangedByInput = false;
	}

	ImGui::Begin("Settings");

	ImGui::InputFloat("Setting Width", &size.x, 1.0f);
	if (ImGui::IsItemDeactivatedAfterEdit()) sizeChangedByInput = true;
	ImGui::InputFloat("Setting Height", &size.y, 1.0f);
	if (ImGui::IsItemDeactivatedAfterEdit()) sizeChangedByInput = true;

	ImGui::InputFloat("Setting x", &pos.x, 1.0f);
	if (ImGui::IsItemDeactivatedAfterEdit()) posChangedByInput = true;
	ImGui::InputFloat("Setting y", &pos.y, 1.0f);
	if (ImGui::IsItemDeactivatedAfterEdit()) posChangedByInput = true;

	if (!sizeChangedByInput) size = ImGui::GetWindowSize();
	if (!posChangedByInput) pos = ImGui::GetWindowPos();

	// ImGui::Text("Current Position: x = %.1f, y = %.1f", pos.x, pos.y);
	// ImGui::Text("Current Size: %.1f x %.1f", size.x, size.y);

	if (ImGui::CollapsingHeader("Volume Setting",ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SliderFloat("MasterVolume", &masterVolume, 0.0f, 100.0f, "%.0f%%");
		ImGui::SliderFloat("SFXVolume", &SFX_Volume, 0.0f, 100.0f, "%.0f%%");
		ImGui::SliderFloat("BGMVolume", &BGM_Volume, 0.0f, 100.0f, "%.0f%%");
		SetMasterVolume(masterVolume*0.01f);
		SetSFXVolume(SFX_Volume*0.01f);
		SetBGMVolume(BGM_Volume*0.01f);
	}

	if (ImGui::CollapsingHeader("VolumeOutput",ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Master Volume: %.0f%%", GetMasterVolume() * 100.0f);
		ImGui::Text("SFX Volume: %.0f%%", GetSFXVolume() * 100.0f);
		ImGui::Text("BGM Volume: %.0f%%", GetBGMVolume() * 100.0f);
	}
	ImGui::End();

}
