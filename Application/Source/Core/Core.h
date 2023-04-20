#pragma once

#include "Log.h"
#include <assimp/Importer.hpp>

#ifdef WIN32
#	define CC_PLATFORM_WINDOWS
#endif

#ifdef _DEBUG
#	define CC_ENABLE_ASSERTS
#endif


#ifdef CC_ENABLE_ASSERTS
#	define CC_ASSERT(x, ...) { if (!(x)) { CC_ERROR("Assertion Failed - ", __VA_ARGS__); __debugbreak(); } }
#else
#	define CC_ASSERT(x, ...)
#endif

#define CC_ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals /*| aiProcess_FlipUVs */ | aiProcess_CalcTangentSpace)

// Bitshift to the left (multiply by 2)
#define BIT(x) (1 << (x))

#define CC_BIND_EVENT_FUNC(fn) std::bind(&fn, this, std::placeholders::_1)


