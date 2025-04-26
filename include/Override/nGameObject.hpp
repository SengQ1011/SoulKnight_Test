//
// Created by QuzzS on 2025/3/1.
//

//nGameObject.hpp

#ifndef NGAMEOBJECT_HPP
#define NGAMEOBJECT_HPP

#include "Util/GameObject.hpp"
#include "Components/Component.hpp"

class nGameObject : public Util::GameObject, public std::enable_shared_from_this<nGameObject> //爲了讓Component可以指向nGameObject
{
public:
	explicit nGameObject(const std::string& objectName = "")
		: m_Name(objectName.empty() ? "GameObject_" + std::to_string(GetNextId()) : objectName ) {}

	~nGameObject() override = default;

	glm::vec2 m_WorldCoord = {0,0};

	// Components
	template <typename T, typename... Args>
	std::shared_ptr<T> AddComponent(ComponentType type, Args &&...args);
	template <typename T>
	std::shared_ptr<T> GetComponent(ComponentType type);

	virtual void Update()
	{
		if (!m_Active) return;

		for (auto& [type, component] : m_Components) {
			component->Update();  // 更新每個組件
		}
	}
	virtual void onCollision(const std::shared_ptr<nGameObject> &other, CollisionInfo &info)
	{
		if (!m_Active) return;

		for (const auto& pair : m_Components) {
			// pair.first 是 ComponentType 键
			// pair.second 是 std::shared_ptr<Component> 值
			pair.second->HandleCollision(info);
		}
	}

	//Setter
	void SetWorldCoord(const glm::vec2 coord) {m_WorldCoord = coord + m_PosOffset;} // 圖片位移補償
	void SetActive(const bool active) { m_Active = active; }
	void SetInitialScale(const glm::vec2& scale) { m_InitialScale = scale; }
	void SetInitialScaleSet(const bool set) { m_InitialScaleSet = set; }
	void SetZIndexType(const ZIndexType zIndexType) {m_ZIndex = zIndexType;}
	void SetPosOffset(const glm::vec2& offset) { m_PosOffset = offset; }
	void SetRegisteredToScene(const bool signal) {m_RegisteredToScene = signal;}

	// Getter
	[[nodiscard]] virtual std::string GetName() const { return m_Name; }
	[[nodiscard]] virtual std::string GetClassName() const { return "nGameObject"; }
	[[nodiscard]] glm::vec2 GetPivot() const { return m_Pivot;}
	[[nodiscard]] glm::vec2 GetImageSize() const {return m_Drawable->GetSize();}
	[[nodiscard]] glm::vec2 GetWorldCoord() const {return m_WorldCoord;}
	[[nodiscard]] std::shared_ptr<Core::Drawable> GetDrawable() const {return m_Drawable;}
	[[nodiscard]] bool IsActive() const { return m_Active; }
	[[nodiscard]] bool IsVisible() const { return m_Visible; }
	[[nodiscard]] glm::vec2 GetInitialScale() const { return m_InitialScale;}
	[[nodiscard]] bool isSetInitialScale()  const { return m_InitialScaleSet;}
	[[nodiscard]] ZIndexType GetZIndexType() const { return m_ZIndex;}
	[[nodiscard]] glm::vec2 GetPosOffset() const { return m_PosOffset;}
	bool IsRegisteredToScene() const { return m_RegisteredToScene; }

protected:
	std::string m_Name;
	bool m_Active = true;

	glm::vec2 m_PosOffset = glm::vec2(0.0f);
	ZIndexType m_ZIndex = ZIndexType::OBJECTHIGH; // 設置ZIndex層，動態調整


	glm::vec2 m_InitialScale;			// 儲存初始縮放
	bool m_InitialScaleSet = false;		// 標記是否已設置初始縮放

	std::unordered_map<ComponentType, std::shared_ptr<Component>> m_Components;

	bool m_RegisteredToScene = false;

private:
	static int GetNextId()
	{
		static int gameObjectCounter = 0;
		return gameObjectCounter++;
	}
};

#include "nGameObject.inl" //必須先完成定義后才能包含

#endif //NGAMEOBJECT_HPP
