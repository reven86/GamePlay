#include "Base.h"
#include "lua_ProgressBarOrientationType.h"

namespace gameplay
{

static const char* enumStringEmpty = "";

static const char* luaEnumString_ProgressBarOrientationType_ORIENTATION_HORIZONTAL = "ORIENTATION_HORIZONTAL";
static const char* luaEnumString_ProgressBarOrientationType_ORIENTATION_VERTICAL = "ORIENTATION_VERTICAL";

ProgressBar::OrientationType lua_enumFromString_ProgressBarOrientationType(const char* s)
{
    if (strcmp(s, luaEnumString_ProgressBarOrientationType_ORIENTATION_HORIZONTAL) == 0)
        return ProgressBar::ORIENTATION_HORIZONTAL;
    if (strcmp(s, luaEnumString_ProgressBarOrientationType_ORIENTATION_VERTICAL) == 0)
        return ProgressBar::ORIENTATION_VERTICAL;
    GP_ERROR("Invalid enumeration value '%s' for enumeration ProgressBar::OrientationType.", s);
    return ProgressBar::ORIENTATION_HORIZONTAL;
}

const char* lua_stringFromEnum_ProgressBarOrientationType(ProgressBar::OrientationType e)
{
    if (e == ProgressBar::ORIENTATION_HORIZONTAL)
        return luaEnumString_ProgressBarOrientationType_ORIENTATION_HORIZONTAL;
    if (e == ProgressBar::ORIENTATION_VERTICAL)
        return luaEnumString_ProgressBarOrientationType_ORIENTATION_VERTICAL;
    GP_ERROR("Invalid enumeration value '%d' for enumeration ProgressBar::OrientationType.", e);
    return enumStringEmpty;
}

}

