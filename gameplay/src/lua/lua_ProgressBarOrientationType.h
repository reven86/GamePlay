#ifndef LUA_PROGRESSBARORIENTATIONTYPE_H_
#define LUA_PROGRESSBARORIENTATIONTYPE_H_

#include "ProgressBar.h"

namespace gameplay
{

// Lua bindings for enum conversion functions for ProgressBar::OrientationType.
ProgressBar::OrientationType lua_enumFromString_ProgressBarOrientationType(const char* s);
const char* lua_stringFromEnum_ProgressBarOrientationType(ProgressBar::OrientationType e);

}

#endif
