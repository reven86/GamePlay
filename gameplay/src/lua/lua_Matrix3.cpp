#include "Base.h"
#include "ScriptController.h"
#include "lua_Matrix3.h"
#include "Base.h"
#include "MathUtil.h"
#include "Matrix3.h"

namespace gameplay
{

void luaRegister_Matrix3()
{
    const luaL_Reg lua_members[] = 
    {
        {"add", lua_Matrix3_add},
        {"decompose", lua_Matrix3_decompose},
        {"determinant", lua_Matrix3_determinant},
        {"getDownVector", lua_Matrix3_getDownVector},
        {"getLeftVector", lua_Matrix3_getLeftVector},
        {"getRightVector", lua_Matrix3_getRightVector},
        {"getRotation", lua_Matrix3_getRotation},
        {"getScale", lua_Matrix3_getScale},
        {"getTranslation", lua_Matrix3_getTranslation},
        {"getUpVector", lua_Matrix3_getUpVector},
        {"invert", lua_Matrix3_invert},
        {"isIdentity", lua_Matrix3_isIdentity},
        {"m", lua_Matrix3_m},
        {"multiply", lua_Matrix3_multiply},
        {"negate", lua_Matrix3_negate},
        {"rotate", lua_Matrix3_rotate},
        {"scale", lua_Matrix3_scale},
        {"set", lua_Matrix3_set},
        {"setIdentity", lua_Matrix3_setIdentity},
        {"setZero", lua_Matrix3_setZero},
        {"subtract", lua_Matrix3_subtract},
        {"transformPoint", lua_Matrix3_transformPoint},
        {"transformVector", lua_Matrix3_transformVector},
        {"translate", lua_Matrix3_translate},
        {"transpose", lua_Matrix3_transpose},
        {NULL, NULL}
    };
    const luaL_Reg lua_statics[] = 
    {
        {"add", lua_Matrix3_static_add},
        {"createRotation", lua_Matrix3_static_createRotation},
        {"createScale", lua_Matrix3_static_createScale},
        {"createTranslation", lua_Matrix3_static_createTranslation},
        {"identity", lua_Matrix3_static_identity},
        {"multiply", lua_Matrix3_static_multiply},
        {"subtract", lua_Matrix3_static_subtract},
        {"zero", lua_Matrix3_static_zero},
        {NULL, NULL}
    };
    std::vector<std::string> scopePath;

    ScriptUtil::registerClass("Matrix3", lua_members, lua_Matrix3__init, lua_Matrix3__gc, lua_statics, scopePath);
}

static Matrix3* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "Matrix3");
    luaL_argcheck(state, userdata != NULL, 1, "'Matrix3' expected.");
    return (Matrix3*)((ScriptUtil::LuaObject*)userdata)->instance;
}

int lua_Matrix3__gc(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                void* userdata = luaL_checkudata(state, 1, "Matrix3");
                luaL_argcheck(state, userdata != NULL, 1, "'Matrix3' expected.");
                ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)userdata;
                if (object->owns)
                {
                    Matrix3* instance = (Matrix3*)object->instance;
                    SAFE_DELETE(instance);
                }
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3__gc - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3__init(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 0:
        {
            void* returnPtr = (void*)new Matrix3();
            if (returnPtr)
            {
                ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                object->instance = returnPtr;
                object->owns = true;
                luaL_getmetatable(state, "Matrix3");
                lua_setmetatable(state, -2);
            }
            else
            {
                lua_pushnil(state);
            }

            return 1;
            break;
        }
        case 1:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TTABLE || lua_type(state, 1) == LUA_TLIGHTUSERDATA))
                {
                    // Get parameter 1 off the stack.
                    ScriptUtil::LuaArray<float> param1 = ScriptUtil::getFloatPointer(1);

                    void* returnPtr = (void*)new Matrix3(param1);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Matrix3");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Matrix3> param1 = ScriptUtil::getObjectPointer<Matrix3>(1, "Matrix3", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    void* returnPtr = (void*)new Matrix3(*param1);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Matrix3");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3__init - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 9:
        {
            do
            {
                if (lua_type(state, 1) == LUA_TNUMBER &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    lua_type(state, 4) == LUA_TNUMBER &&
                    lua_type(state, 5) == LUA_TNUMBER &&
                    lua_type(state, 6) == LUA_TNUMBER &&
                    lua_type(state, 7) == LUA_TNUMBER &&
                    lua_type(state, 8) == LUA_TNUMBER &&
                    lua_type(state, 9) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 1);

                    // Get parameter 2 off the stack.
                    float param2 = (float)luaL_checknumber(state, 2);

                    // Get parameter 3 off the stack.
                    float param3 = (float)luaL_checknumber(state, 3);

                    // Get parameter 4 off the stack.
                    float param4 = (float)luaL_checknumber(state, 4);

                    // Get parameter 5 off the stack.
                    float param5 = (float)luaL_checknumber(state, 5);

                    // Get parameter 6 off the stack.
                    float param6 = (float)luaL_checknumber(state, 6);

                    // Get parameter 7 off the stack.
                    float param7 = (float)luaL_checknumber(state, 7);

                    // Get parameter 8 off the stack.
                    float param8 = (float)luaL_checknumber(state, 8);

                    // Get parameter 9 off the stack.
                    float param9 = (float)luaL_checknumber(state, 9);

                    void* returnPtr = (void*)new Matrix3(param1, param2, param3, param4, param5, param6, param7, param8, param9);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Matrix3");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3__init - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 0, 1 or 9).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_add(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    Matrix3* instance = getInstance(state);
                    instance->add(param1);
                    
                    return 0;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Matrix3> param1 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->add(*param1);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_add - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 3:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<Matrix3> param2 = ScriptUtil::getObjectPointer<Matrix3>(3, "Matrix3", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->add(param1, param2);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_add - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2 or 3).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_decompose(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 4:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL) &&
                (lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TLIGHTUSERDATA) &&
                (lua_type(state, 4) == LUA_TUSERDATA || lua_type(state, 4) == LUA_TTABLE || lua_type(state, 4) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", false, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'Vector2'.");
                    lua_error(state);
                }

                // Get parameter 2 off the stack.
                ScriptUtil::LuaArray<float> param2 = ScriptUtil::getFloatPointer(3);

                // Get parameter 3 off the stack.
                bool param3Valid;
                ScriptUtil::LuaArray<Vector2> param3 = ScriptUtil::getObjectPointer<Vector2>(4, "Vector2", false, &param3Valid);
                if (!param3Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 3 to type 'Vector2'.");
                    lua_error(state);
                }

                Matrix3* instance = getInstance(state);
                bool result = instance->decompose(param1, param2, param3);

                // Push the return value onto the stack.
                lua_pushboolean(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Matrix3_decompose - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 4).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_determinant(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Matrix3* instance = getInstance(state);
                float result = instance->determinant();

                // Push the return value onto the stack.
                lua_pushnumber(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Matrix3_determinant - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_getDownVector(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", false, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'Vector2'.");
                    lua_error(state);
                }

                Matrix3* instance = getInstance(state);
                instance->getDownVector(param1);
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3_getDownVector - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_getLeftVector(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", false, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'Vector2'.");
                    lua_error(state);
                }

                Matrix3* instance = getInstance(state);
                instance->getLeftVector(param1);
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3_getLeftVector - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_getRightVector(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", false, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'Vector2'.");
                    lua_error(state);
                }

                Matrix3* instance = getInstance(state);
                instance->getRightVector(param1);
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3_getRightVector - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_getRotation(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TLIGHTUSERDATA))
            {
                // Get parameter 1 off the stack.
                ScriptUtil::LuaArray<float> param1 = ScriptUtil::getFloatPointer(2);

                Matrix3* instance = getInstance(state);
                bool result = instance->getRotation(param1);

                // Push the return value onto the stack.
                lua_pushboolean(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Matrix3_getRotation - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_getScale(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", false, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'Vector2'.");
                    lua_error(state);
                }

                Matrix3* instance = getInstance(state);
                instance->getScale(param1);
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3_getScale - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_getTranslation(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", false, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'Vector2'.");
                    lua_error(state);
                }

                Matrix3* instance = getInstance(state);
                instance->getTranslation(param1);
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3_getTranslation - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_getUpVector(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", false, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'Vector2'.");
                    lua_error(state);
                }

                Matrix3* instance = getInstance(state);
                instance->getUpVector(param1);
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3_getUpVector - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_invert(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA))
                {
                    Matrix3* instance = getInstance(state);
                    bool result = instance->invert();

                    // Push the return value onto the stack.
                    lua_pushboolean(state, result);

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_invert - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Matrix3> param1 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", false, &param1Valid);
                    if (!param1Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    bool result = instance->invert(param1);

                    // Push the return value onto the stack.
                    lua_pushboolean(state, result);

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_invert - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1 or 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_isIdentity(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Matrix3* instance = getInstance(state);
                bool result = instance->isIdentity();

                // Push the return value onto the stack.
                lua_pushboolean(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Matrix3_isIdentity - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_m(lua_State* state)
{
    // Validate the number of parameters.
    if (lua_gettop(state) > 2)
    {
        lua_pushstring(state, "Invalid number of parameters (expected 1 or 2).");
        lua_error(state);
    }

    Matrix3* instance = getInstance(state);
    if (lua_gettop(state) == 2)
    {
        // Get parameter 2 off the stack.
        ScriptUtil::LuaArray<float> param2 = ScriptUtil::getFloatPointer(2);

        memcpy(instance->m, param2, sizeof(float) * 9);
        return 0;
    }
    else
    {
        float* result = instance->m;

        // Push the return value onto the stack.
        lua_pushlightuserdata(state, result);
        return 1;
    }
}

int lua_Matrix3_multiply(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    Matrix3* instance = getInstance(state);
                    instance->multiply(param1);
                    
                    return 0;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Matrix3> param1 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->multiply(*param1);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_multiply - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 3:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<Matrix3> param2 = ScriptUtil::getObjectPointer<Matrix3>(3, "Matrix3", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->multiply(param1, param2);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_multiply - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2 or 3).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_negate(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA))
                {
                    Matrix3* instance = getInstance(state);
                    instance->negate();
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_negate - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Matrix3> param1 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", false, &param1Valid);
                    if (!param1Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->negate(param1);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_negate - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1 or 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_rotate(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    Matrix3* instance = getInstance(state);
                    instance->rotate(param1);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_rotate - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 3:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<Matrix3> param2 = ScriptUtil::getObjectPointer<Matrix3>(3, "Matrix3", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->rotate(param1, param2);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_rotate - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2 or 3).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_scale(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    Matrix3* instance = getInstance(state);
                    instance->scale(param1);
                    
                    return 0;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->scale(*param1);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_scale - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 3:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<Matrix3> param2 = ScriptUtil::getObjectPointer<Matrix3>(3, "Matrix3", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->scale(param1, param2);
                    
                    return 0;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    // Get parameter 2 off the stack.
                    float param2 = (float)luaL_checknumber(state, 3);

                    Matrix3* instance = getInstance(state);
                    instance->scale(param1, param2);
                    
                    return 0;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<Matrix3> param2 = ScriptUtil::getObjectPointer<Matrix3>(3, "Matrix3", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->scale(*param1, param2);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_scale - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 4:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    (lua_type(state, 4) == LUA_TUSERDATA || lua_type(state, 4) == LUA_TTABLE || lua_type(state, 4) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    // Get parameter 2 off the stack.
                    float param2 = (float)luaL_checknumber(state, 3);

                    // Get parameter 3 off the stack.
                    bool param3Valid;
                    ScriptUtil::LuaArray<Matrix3> param3 = ScriptUtil::getObjectPointer<Matrix3>(4, "Matrix3", false, &param3Valid);
                    if (!param3Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->scale(param1, param2, param3);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_scale - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2, 3 or 4).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_set(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TLIGHTUSERDATA))
                {
                    // Get parameter 1 off the stack.
                    ScriptUtil::LuaArray<float> param1 = ScriptUtil::getFloatPointer(2);

                    Matrix3* instance = getInstance(state);
                    instance->set(param1);
                    
                    return 0;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Matrix3> param1 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->set(*param1);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_set - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 10:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    lua_type(state, 4) == LUA_TNUMBER &&
                    lua_type(state, 5) == LUA_TNUMBER &&
                    lua_type(state, 6) == LUA_TNUMBER &&
                    lua_type(state, 7) == LUA_TNUMBER &&
                    lua_type(state, 8) == LUA_TNUMBER &&
                    lua_type(state, 9) == LUA_TNUMBER &&
                    lua_type(state, 10) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    // Get parameter 2 off the stack.
                    float param2 = (float)luaL_checknumber(state, 3);

                    // Get parameter 3 off the stack.
                    float param3 = (float)luaL_checknumber(state, 4);

                    // Get parameter 4 off the stack.
                    float param4 = (float)luaL_checknumber(state, 5);

                    // Get parameter 5 off the stack.
                    float param5 = (float)luaL_checknumber(state, 6);

                    // Get parameter 6 off the stack.
                    float param6 = (float)luaL_checknumber(state, 7);

                    // Get parameter 7 off the stack.
                    float param7 = (float)luaL_checknumber(state, 8);

                    // Get parameter 8 off the stack.
                    float param8 = (float)luaL_checknumber(state, 9);

                    // Get parameter 9 off the stack.
                    float param9 = (float)luaL_checknumber(state, 10);

                    Matrix3* instance = getInstance(state);
                    instance->set(param1, param2, param3, param4, param5, param6, param7, param8, param9);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_set - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2 or 10).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_setIdentity(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Matrix3* instance = getInstance(state);
                instance->setIdentity();
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3_setIdentity - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_setZero(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Matrix3* instance = getInstance(state);
                instance->setZero();
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3_setZero - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_static_add(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 3:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TNIL) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<Matrix3> param1 = ScriptUtil::getObjectPointer<Matrix3>(1, "Matrix3", true, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'Matrix3'.");
                    lua_error(state);
                }

                // Get parameter 2 off the stack.
                bool param2Valid;
                ScriptUtil::LuaArray<Matrix3> param2 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", true, &param2Valid);
                if (!param2Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 2 to type 'Matrix3'.");
                    lua_error(state);
                }

                // Get parameter 3 off the stack.
                bool param3Valid;
                ScriptUtil::LuaArray<Matrix3> param3 = ScriptUtil::getObjectPointer<Matrix3>(3, "Matrix3", false, &param3Valid);
                if (!param3Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 3 to type 'Matrix3'.");
                    lua_error(state);
                }

                Matrix3::add(*param1, *param2, param3);
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3_static_add - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 3).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_static_createRotation(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if (lua_type(state, 1) == LUA_TNUMBER &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                float param1 = (float)luaL_checknumber(state, 1);

                // Get parameter 2 off the stack.
                bool param2Valid;
                ScriptUtil::LuaArray<Matrix3> param2 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", false, &param2Valid);
                if (!param2Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 2 to type 'Matrix3'.");
                    lua_error(state);
                }

                Matrix3::createRotation(param1, param2);
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3_static_createRotation - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_static_createScale(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TNIL) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(1, "Vector2", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<Matrix3> param2 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    Matrix3::createScale(*param1, param2);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_static_createScale - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 3:
        {
            do
            {
                if (lua_type(state, 1) == LUA_TNUMBER &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 1);

                    // Get parameter 2 off the stack.
                    float param2 = (float)luaL_checknumber(state, 2);

                    // Get parameter 3 off the stack.
                    bool param3Valid;
                    ScriptUtil::LuaArray<Matrix3> param3 = ScriptUtil::getObjectPointer<Matrix3>(3, "Matrix3", false, &param3Valid);
                    if (!param3Valid)
                        break;

                    Matrix3::createScale(param1, param2, param3);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_static_createScale - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2 or 3).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_static_createTranslation(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TNIL) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(1, "Vector2", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<Matrix3> param2 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    Matrix3::createTranslation(*param1, param2);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_static_createTranslation - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 3:
        {
            do
            {
                if (lua_type(state, 1) == LUA_TNUMBER &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 1);

                    // Get parameter 2 off the stack.
                    float param2 = (float)luaL_checknumber(state, 2);

                    // Get parameter 3 off the stack.
                    bool param3Valid;
                    ScriptUtil::LuaArray<Matrix3> param3 = ScriptUtil::getObjectPointer<Matrix3>(3, "Matrix3", false, &param3Valid);
                    if (!param3Valid)
                        break;

                    Matrix3::createTranslation(param1, param2, param3);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_static_createTranslation - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2 or 3).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_static_identity(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 0:
        {
            void* returnPtr = (void*)&(Matrix3::identity());
            if (returnPtr)
            {
                ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                object->instance = returnPtr;
                object->owns = false;
                luaL_getmetatable(state, "Matrix3");
                lua_setmetatable(state, -2);
            }
            else
            {
                lua_pushnil(state);
            }

            return 1;
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 0).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_static_multiply(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 3:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TNIL) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Matrix3> param1 = ScriptUtil::getObjectPointer<Matrix3>(1, "Matrix3", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    float param2 = (float)luaL_checknumber(state, 2);

                    // Get parameter 3 off the stack.
                    bool param3Valid;
                    ScriptUtil::LuaArray<Matrix3> param3 = ScriptUtil::getObjectPointer<Matrix3>(3, "Matrix3", false, &param3Valid);
                    if (!param3Valid)
                        break;

                    Matrix3::multiply(*param1, param2, param3);
                    
                    return 0;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TNIL) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Matrix3> param1 = ScriptUtil::getObjectPointer<Matrix3>(1, "Matrix3", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<Matrix3> param2 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", true, &param2Valid);
                    if (!param2Valid)
                        break;

                    // Get parameter 3 off the stack.
                    bool param3Valid;
                    ScriptUtil::LuaArray<Matrix3> param3 = ScriptUtil::getObjectPointer<Matrix3>(3, "Matrix3", false, &param3Valid);
                    if (!param3Valid)
                        break;

                    Matrix3::multiply(*param1, *param2, param3);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_static_multiply - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 3).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_static_subtract(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 3:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TNIL) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<Matrix3> param1 = ScriptUtil::getObjectPointer<Matrix3>(1, "Matrix3", true, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'Matrix3'.");
                    lua_error(state);
                }

                // Get parameter 2 off the stack.
                bool param2Valid;
                ScriptUtil::LuaArray<Matrix3> param2 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", true, &param2Valid);
                if (!param2Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 2 to type 'Matrix3'.");
                    lua_error(state);
                }

                // Get parameter 3 off the stack.
                bool param3Valid;
                ScriptUtil::LuaArray<Matrix3> param3 = ScriptUtil::getObjectPointer<Matrix3>(3, "Matrix3", false, &param3Valid);
                if (!param3Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 3 to type 'Matrix3'.");
                    lua_error(state);
                }

                Matrix3::subtract(*param1, *param2, param3);
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3_static_subtract - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 3).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_static_zero(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 0:
        {
            void* returnPtr = (void*)&(Matrix3::zero());
            if (returnPtr)
            {
                ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                object->instance = returnPtr;
                object->owns = false;
                luaL_getmetatable(state, "Matrix3");
                lua_setmetatable(state, -2);
            }
            else
            {
                lua_pushnil(state);
            }

            return 1;
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 0).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_subtract(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                ScriptUtil::LuaArray<Matrix3> param1 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", true, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'Matrix3'.");
                    lua_error(state);
                }

                Matrix3* instance = getInstance(state);
                instance->subtract(*param1);
                
                return 0;
            }

            lua_pushstring(state, "lua_Matrix3_subtract - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_transformPoint(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", false, &param1Valid);
                    if (!param1Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->transformPoint(param1);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_transformPoint - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 3:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<Vector2> param2 = ScriptUtil::getObjectPointer<Vector2>(3, "Vector2", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->transformPoint(*param1, param2);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_transformPoint - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2 or 3).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_transformVector(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", false, &param1Valid);
                    if (!param1Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->transformVector(param1);
                    
                    return 0;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Vector3> param1 = ScriptUtil::getObjectPointer<Vector3>(2, "Vector3", false, &param1Valid);
                    if (!param1Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->transformVector(param1);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_transformVector - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 3:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<Vector2> param2 = ScriptUtil::getObjectPointer<Vector2>(3, "Vector2", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->transformVector(*param1, param2);
                    
                    return 0;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Vector3> param1 = ScriptUtil::getObjectPointer<Vector3>(2, "Vector3", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<Vector3> param2 = ScriptUtil::getObjectPointer<Vector3>(3, "Vector3", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->transformVector(*param1, param2);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_transformVector - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 5:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    lua_type(state, 4) == LUA_TNUMBER &&
                    (lua_type(state, 5) == LUA_TUSERDATA || lua_type(state, 5) == LUA_TTABLE || lua_type(state, 5) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    // Get parameter 2 off the stack.
                    float param2 = (float)luaL_checknumber(state, 3);

                    // Get parameter 3 off the stack.
                    float param3 = (float)luaL_checknumber(state, 4);

                    // Get parameter 4 off the stack.
                    bool param4Valid;
                    ScriptUtil::LuaArray<Vector2> param4 = ScriptUtil::getObjectPointer<Vector2>(5, "Vector2", false, &param4Valid);
                    if (!param4Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->transformVector(param1, param2, param3, param4);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_transformVector - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2, 3 or 5).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_translate(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->translate(*param1);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_translate - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 3:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    // Get parameter 2 off the stack.
                    float param2 = (float)luaL_checknumber(state, 3);

                    Matrix3* instance = getInstance(state);
                    instance->translate(param1, param2);
                    
                    return 0;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL) &&
                    (lua_type(state, 3) == LUA_TUSERDATA || lua_type(state, 3) == LUA_TTABLE || lua_type(state, 3) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Vector2> param1 = ScriptUtil::getObjectPointer<Vector2>(2, "Vector2", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    bool param2Valid;
                    ScriptUtil::LuaArray<Matrix3> param2 = ScriptUtil::getObjectPointer<Matrix3>(3, "Matrix3", false, &param2Valid);
                    if (!param2Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->translate(*param1, param2);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_translate - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 4:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    (lua_type(state, 4) == LUA_TUSERDATA || lua_type(state, 4) == LUA_TTABLE || lua_type(state, 4) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    float param1 = (float)luaL_checknumber(state, 2);

                    // Get parameter 2 off the stack.
                    float param2 = (float)luaL_checknumber(state, 3);

                    // Get parameter 3 off the stack.
                    bool param3Valid;
                    ScriptUtil::LuaArray<Matrix3> param3 = ScriptUtil::getObjectPointer<Matrix3>(4, "Matrix3", false, &param3Valid);
                    if (!param3Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->translate(param1, param2, param3);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_translate - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2, 3 or 4).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Matrix3_transpose(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA))
                {
                    Matrix3* instance = getInstance(state);
                    instance->transpose();
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_transpose - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                    (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Matrix3> param1 = ScriptUtil::getObjectPointer<Matrix3>(2, "Matrix3", false, &param1Valid);
                    if (!param1Valid)
                        break;

                    Matrix3* instance = getInstance(state);
                    instance->transpose(param1);
                    
                    return 0;
                }
            } while (0);

            lua_pushstring(state, "lua_Matrix3_transpose - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1 or 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

}
