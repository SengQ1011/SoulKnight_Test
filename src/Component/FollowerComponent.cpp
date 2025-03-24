//
// Created by QuzzS on 2025/3/18.
//

//FollowerComponent.cpp

#include "Components/FollowerComponent.hpp"

#include "Cursor.hpp"
#include "Override/nGameObject.hpp"
#include "Util/Input.hpp"
void FollowerComponent::BaseTargetRotate()
{
	const auto owner = GetOwner<nGameObject>();
	//TODO:m_Owner坐標嗎？，還是握把坐標？ 彈道好像也可以用這個？
	//	   1. 轉角最大幅度 PI ~ -PI
	//	   2. 超過就要反轉x軸了 --> 什麽時候角色武器要反轉？ 面向目標（敵人/鼠標）->目標位置和角色位置差，當差的x<0就是要反轉了 --> 在角色實作反轉，武器被影響
	if (m_Target)
	{
		const auto targetWorldCoord = m_Target->GetWorldCoord();
		m_HoldingRotation = glm::atan(targetWorldCoord.y - owner->m_WorldCoord.y, targetWorldCoord.x - owner->m_WorldCoord.x);
	}
	else if (m_UseMousePosition)
	{
		const auto targetWorldCoord = Cursor::GetCursorWorldCoord(owner->m_Transform.scale.x);
		m_HoldingRotation = glm::atan(targetWorldCoord.y - owner->m_WorldCoord.y, targetWorldCoord.x - owner->m_WorldCoord.x);
	}
	else
	{
		LOG_DEBUG("FollowerComponent::BaseTargetRotate");
	}
}


void FollowerComponent::Update()
{
	const auto owner = GetOwner<nGameObject>();
	//TODO:當角色反轉，m_HandOffset.x會被影響， m_HoldingOffset不確定，可能要實驗
	//Follower的手是m_Owner的世界坐標
	//m_Owner的轉軸是m_Owner的世界坐標-它到握把的距離
	owner->m_WorldCoord = m_Follower->m_WorldCoord + m_HandOffset;
	owner->SetPivot(-m_HoldingOffset);
	BaseTargetRotate(); //取得角度
	owner->m_Transform.rotation = m_HoldingRotation;
}

