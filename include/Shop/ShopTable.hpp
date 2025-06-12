//
// Created by AI Assistant on 2025/4/29.
//

#ifndef SHOPTABLE_HPP
#define SHOPTABLE_HPP

#include <memory>
#include <string>
#include "EnumTypes.hpp"
#include "Override/nGameObject.hpp"


// 前向聲明
class InteractableComponent;
class Character;

// 商品類型
enum class ShopItemType
{
	HEALTH_POTION,
	ENERGY_POTION,
	WEAPON
};

// 藥水大小枚舉已移至 EnumTypes.hpp

class ShopTable : public nGameObject
{
public:
	ShopTable(const std::string &baseName = "", const std::string &baseClass = "nGameObject");
	~ShopTable() override = default;

	// 初始化方法
	void Initialize(const std::string &name, ShopItemType itemType, int price,
					PotionSize potionSize = PotionSize::SMALL);

	// 更新方法
	void Update() override;

	// 商品管理
	void CreateItem(const std::shared_ptr<Character> &player);
	void SetItem(const std::shared_ptr<nGameObject> &item) { m_CurrentItem = item; }
	void RefreshItem(const std::shared_ptr<Character> &player); // 重新生成商品

	// 購買邏輯
	bool CanPurchase(const std::shared_ptr<Character> &player) const;
	bool PurchaseItem(const std::shared_ptr<Character> &player);

	// 取得商品資訊
	ShopItemType GetItemType() const { return m_ItemType; }
	int GetPrice() const { return m_Price; }
	bool HasItem() const { return m_CurrentItem != nullptr; }
	std::shared_ptr<nGameObject> GetCurrentItem() const { return m_CurrentItem; }
	PotionSize GetPotionSize() const { return m_PotionSize; }

	// 設置方法
	void SetItemType(ShopItemType itemType) { m_ItemType = itemType; }
	void SetPrice(int price) { m_Price = price; }
	void SetCurrentItem(std::shared_ptr<nGameObject> item) { m_CurrentItem = item; }
	void SetPotionSize(PotionSize potionSize) { m_PotionSize = potionSize; }

	// 清理當前商品並重新啟用商店桌子
	void ClearCurrentItem();

protected:
	void CreateHealthPotion();
	void CreateEnergyPotion();
	void CreateRandomWeapon(const std::shared_ptr<Character> &player);

	void RemoveCurrentItem();

private:
	ShopItemType m_ItemType;
	int m_Price;
	PotionSize m_PotionSize = PotionSize::SMALL; // 藥水大小（僅對藥水類型有效）
	std::shared_ptr<nGameObject> m_CurrentItem;
	std::shared_ptr<InteractableComponent> m_InteractableComponent;

	// 商品生成偏移
	glm::vec2 m_ItemOffset = glm::vec2(0, 10); // 商品在桌子上方20像素
};

#endif // SHOPTABLE_HPP
