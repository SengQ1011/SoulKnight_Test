//
// Created by QuzzS on 2025/4/26.
//

// Util/MemoryTracker.hpp

#pragma once
#include <cstdlib> // malloc, free

#if defined(TRACY_ENABLE) && defined(TRACY_MEMORY)
#include "Tracy.hpp"

inline void* operator new(std::size_t size)
{
	void* ptr = std::malloc(size);
	TracyAlloc(ptr, size);
	return ptr;
}

inline void operator delete(void* ptr) noexcept
{
	TracyFree(ptr);
	std::free(ptr);
}
#endif

