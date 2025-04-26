//
// Created by QuzzS on 2025/4/26.
//

#ifndef PROFILER_HPP
#define PROFILER_HPP

#ifdef ENABLE_TRACY
	#include "Tracy.hpp"
	#define ZONE_SCOPE(name) ZoneScopedN(name)
	#define ZONE_FRAME_MARK FrameMark
#else
	#define ZONE_SCOPE(name)
	#define ZONE_FRAME_MARK
#endif

#endif //PROFILER_HPP
