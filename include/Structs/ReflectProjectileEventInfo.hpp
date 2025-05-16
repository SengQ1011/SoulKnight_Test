//
// Created by tjx20 on 5/9/2025.
//

#ifndef REFLECTPROJECTILEEVENTINFO_HPP
#define REFLECTPROJECTILEEVENTINFO_HPP

#include "EventInfo.hpp"

struct ReflectProjectileEventInfo final : TypedEventInfo<ReflectProjectileEventInfo>
{
	explicit ReflectProjectileEventInfo() :
		TypedEventInfo(EventType::ReflectProjectile) {}
};

#endif //REFLECTPROJECTILEEVENTINFO_HPP
