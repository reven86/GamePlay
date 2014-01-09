#include "Base.h"
#include "lua_RenderStateFrontFace.h"

namespace gameplay
{

static const char* enumStringEmpty = "";

static const char* luaEnumString_RenderStateFrontFace_FRONT_FACE_CW = "FRONT_FACE_CW";
static const char* luaEnumString_RenderStateFrontFace_FRONT_FACE_CCW = "FRONT_FACE_CCW";

RenderState::FrontFace lua_enumFromString_RenderStateFrontFace(const char* s)
{
    if (strcmp(s, luaEnumString_RenderStateFrontFace_FRONT_FACE_CW) == 0)
        return RenderState::FRONT_FACE_CW;
    if (strcmp(s, luaEnumString_RenderStateFrontFace_FRONT_FACE_CCW) == 0)
        return RenderState::FRONT_FACE_CCW;
    GP_ERROR("Invalid enumeration value '%s' for enumeration RenderState::FrontFace.", s);
    return RenderState::FRONT_FACE_CW;
}

const char* lua_stringFromEnum_RenderStateFrontFace(RenderState::FrontFace e)
{
    if (e == RenderState::FRONT_FACE_CW)
        return luaEnumString_RenderStateFrontFace_FRONT_FACE_CW;
    if (e == RenderState::FRONT_FACE_CCW)
        return luaEnumString_RenderStateFrontFace_FRONT_FACE_CCW;
    GP_ERROR("Invalid enumeration value '%d' for enumeration RenderState::FrontFace.", e);
    return enumStringEmpty;
}

}

