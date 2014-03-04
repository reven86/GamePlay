#ifndef LUA_MATRIX3_H_
#define LUA_MATRIX3_H_

namespace gameplay
{

// Lua bindings for Matrix3.
int lua_Matrix3__gc(lua_State* state);
int lua_Matrix3__init(lua_State* state);
int lua_Matrix3_add(lua_State* state);
int lua_Matrix3_decompose(lua_State* state);
int lua_Matrix3_determinant(lua_State* state);
int lua_Matrix3_getDownVector(lua_State* state);
int lua_Matrix3_getLeftVector(lua_State* state);
int lua_Matrix3_getRightVector(lua_State* state);
int lua_Matrix3_getRotation(lua_State* state);
int lua_Matrix3_getScale(lua_State* state);
int lua_Matrix3_getTranslation(lua_State* state);
int lua_Matrix3_getUpVector(lua_State* state);
int lua_Matrix3_invert(lua_State* state);
int lua_Matrix3_isIdentity(lua_State* state);
int lua_Matrix3_m(lua_State* state);
int lua_Matrix3_multiply(lua_State* state);
int lua_Matrix3_negate(lua_State* state);
int lua_Matrix3_rotate(lua_State* state);
int lua_Matrix3_scale(lua_State* state);
int lua_Matrix3_set(lua_State* state);
int lua_Matrix3_setIdentity(lua_State* state);
int lua_Matrix3_setZero(lua_State* state);
int lua_Matrix3_static_add(lua_State* state);
int lua_Matrix3_static_createRotation(lua_State* state);
int lua_Matrix3_static_createScale(lua_State* state);
int lua_Matrix3_static_createTranslation(lua_State* state);
int lua_Matrix3_static_identity(lua_State* state);
int lua_Matrix3_static_multiply(lua_State* state);
int lua_Matrix3_static_subtract(lua_State* state);
int lua_Matrix3_static_zero(lua_State* state);
int lua_Matrix3_subtract(lua_State* state);
int lua_Matrix3_transformPoint(lua_State* state);
int lua_Matrix3_transformVector(lua_State* state);
int lua_Matrix3_translate(lua_State* state);
int lua_Matrix3_transpose(lua_State* state);

void luaRegister_Matrix3();

}

#endif
