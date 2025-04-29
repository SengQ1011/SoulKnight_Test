//
// Created by QuzzS on 2025/4/26.
//

// Util/MemoryTracker.hpp

#pragma once
#include <cstdlib> // malloc, free

#if defined(TRACY_ENABLE) && defined(TRACY_MEMORY)
#include "Tracy.hpp"

/**
 * @brief 重載全局 operator new，讓 Tracy 能追蹤每次記憶體配置的位置和大小
 */
inline void* operator new(std::size_t size)
{
	void* ptr = std::malloc(size);
	TracyAlloc(ptr, size);
	return ptr;
}

/**
 * @brief 重載全局 operator delete，讓 Tracy 能追蹤每次記憶體釋放的位置
 */
inline void operator delete(void* ptr) noexcept
{
	TracyFree(ptr);
	std::free(ptr);
}
#endif

