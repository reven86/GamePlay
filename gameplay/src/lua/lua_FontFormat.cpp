#include "Base.h"
#include "lua_FontFormat.h"

namespace gameplay
{

static const char* enumStringEmpty = "";

static const char* luaEnumString_FontFormat_BITMAP = "BITMAP";
static const char* luaEnumString_FontFormat_DISTANCE_FIELD = "DISTANCE_FIELD";

Font::Format lua_enumFromString_FontFormat(const char* s)
{
    if (strcmp(s, luaEnumString_FontFormat_BITMAP) == 0)
        return Font::BITMAP;
    if (strcmp(s, luaEnumString_FontFormat_DISTANCE_FIELD) == 0)
        return Font::DISTANCE_FIELD;
    GP_ERROR("Invalid enumeration value '%s' for enumeration Font::Format.", s);
    return Font::BITMAP;
}

const char* lua_stringFromEnum_FontFormat(Font::Format e)
{
    if (e == Font::BITMAP)
        return luaEnumString_FontFormat_BITMAP;
    if (e == Font::DISTANCE_FIELD)
        return luaEnumString_FontFormat_DISTANCE_FIELD;
    GP_ERROR("Invalid enumeration value '%d' for enumeration Font::Format.", e);
    return enumStringEmpty;
}

}

