//
// Created by tjx20 on 5/9/2025.
//

#ifndef BLOCKPROJECTILEEVENTINFO_HPP
#define BLOCKPROJECTILEEVENTINFO_HPP

#include "EventInfo.hpp"

struct BlockProjectileEventInfo final : TypedEventInfo<BlockProjectileEventInfo>
{
	explicit BlockProjectileEventInfo() :
		TypedEventInfo(EventType::BlockedProjectileBySword) {}
};

#endif //BLOCKPROJECTILEEVENTINFO_HPP
